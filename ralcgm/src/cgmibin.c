/*   RAL-CGM Interpreter module:  @(#) cgmibin.c  version 3.5
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
 * Description: Read a CGM Binary Encoded file
 *
 * Author:   S C Taylor
 *
 * Maintenance Log:
 *   1 Mar 89 SCT  Original Version stabilised
 *  11 Jul 89 RTP  Random access to frames added
 *  15 Aug 89 RTP  Correct ASF handling
 *  17 Aug 89 RTP  Add ANSI compliant prototypes
 *  18 Aug 89 RTP  Add Cell Arrays and Patterns
 *   9 Oct 89 RTP  Change comarea to arrays pint & preal
 *   3 Nov 89 RTP  Revise control loop to match char & text
 *                 Add new calls to CGMframe
 *   6 Nov 89 RTP  Add lookup table for Reals
 *  23 Nov 89 RTP  Add code for partitioned elements
 *  23 Jan 90 RTP  Correct run-length cell arrays
 *  25 Jan 90 RTP  Handling run length precisions < 8
 *  25 Mar 90 RTP  Change ERRORMESS macro to set micerror
 *                 Check for insufficient points in LINE etc
 *  23 May 90 RTP  Correct partition length calculation
 *            RTP  Check for buffer full and send to output
 *  25 Jul 90 RTP  Incorporate include files in cgmin.h
 *                 Change to use structure curibin
 *                 Add routine CGMdefaults
 *  10 Aug 90 RTP  Reset mibbytes if file has been repositioned
 *                 ie  CGMframe return is non-zero
 *   6 Sep 90 RTP  Change internal routines to static
 *  19 Sep 90 RTP  Remove unused variables and other lint problems
 *   4 Oct 90 RTP  Improve casting after testing with Microsoft
 *   8 Feb 91 RTP  Change loop control in MIBpbody to only exit on ENDMF
 *                 to allow NONOP in Picture Body
 *  15 Mar 91 RTP  Change cgmstate to Enum type
 *  19 Mar 91 RTP  Add CGM code to Error message for Invalid code
 *  22 Apr 91 RTP  Add CGMdefaults routine at BEGPICBODY
 *  14 May 91 RTP  Add ANSI declarations
 *  29 Jul 91 RTP  Correct problem with FONTLIST when odd parameter length
 *   2 Aug 91 RTP  Add SCCS id
 *   7 Aug 91 RTP  Change MFVERSION check to use CGMVERSION
 *  22 Oct 91 RTP  Remove odd byte before random frame access
 *  17 Jul 92 RTP  Add code to handle both old and correct -ve integers
 *  12 Aug 92 RTP  Add check for continuation to prevent clearing input
 *                 buffer
 *  27 Aug 92 RTP  Correct FONT & TEXTINDEX to use Index precision
 *                 Set real precisions using *_bits and *_places
 *  13 Oct 92 RTP  Correct first assignment after Pattern Table
 *  12 Nov 92 RTP  Add Clip rectangle flag
 *                 Change arguments to CGMialloc etc to specify size
 *   9 Feb 94 RTP  Change 1<<n to 1L<<n where necessary
 *  10 Feb 94 RTP  Move definition of DMESS to cgmmach.h
 *  12 Aug 94 KEVP Enable interpreter to finish with a return rather than an exit
 *  30 Nov 94 KEVP Use pow function for very high (vdc)real precisions.
 *  22 Feb 95 KEVP At end of  MIBpbody, clear any remaining space,
 *                 if *pint is non-negative rather than just positive.
 *                 This is necessary because *pint may be zero after
 *                 a non-final text element.
 *                 Continuation occurs only if *pint is negative.
 *   2 Jun 95 KEVP In MIBreal, interpret the precision argument as half the total number
 *                 of bits (because it comes from cur.real_bits or cur.vdc_bits).
 *   8 Jun 95 KEVP If MIBcichar is unable to translate opcode, report unknown element
 *                 rather than invalid error (as with CGMItext).
 *   9 Jun 95 KEVP Use pow function for(VDC)REALPREC for maximum real, when precision
 *                 EQUAL to length of long integer.
 *  16 Jun 95 KEVP Read negative (VDC)reals as in CGM standard (with +ve fraction)
 */

#include "cgmin.h"

/*  Output Driver  */

/* File used */

extern FILE *cgmi;

/* Local macros */

#define NEXTCODE(c)  ( c = ( c ? c : MIBopcode() ) )

#define GETINT(x)    MIBint(curibin.x, SIGNED)
#define GETENUM      MIBint(ENUMERATED, SIGNED)
#define GETREAL      MIBreal( curibin.real_type, cur.real_bits)
#define GETVDC       MIBreal( curibin.vdc_type, cur.vdc_bits)

/*  Local Parameters  */

#define FLOATING     (Enum) 0
#define FIXED        (Enum) 1

#define UNSIGNED     (Enum) 0
#define SIGNED       (Enum) 1

#define ENUMERATED   (Prec) 16
#define BYTE         (Prec) 8
#define WORD         (Prec) 16

#define RUNLENGTH    (Enum) 0
#define PACKED       (Enum) 1

/*  Constants for real conversions */

#define TWO16 (Double) (1L<<16)       /* 65536.0 */
#define TWO32 (Double) (TWO16*TWO16)  /* 4294967296.0 */
#define TWO23 (Double) (1L<<23)       /*  8388608 */
#define TWO52 (Double) (TWO23*(1L<<29))

/*  Routines in this Module */

#ifdef PROTO
   void CGMIbin( void );
   static Code MIBmfdesc( Code ),
               MIBpdesc( Code c, Logical single ),
               MIBpbody( Code c, Logical single );
   static void MIBtext( Code ),
               MIBexternal( Code ),
               MIBstring( char* ),
               MIBvdc( Int, Long*, Float* ),
               MIBclist( Long *pi, Long n, Prec prec, Enum type ),
               MIBcolour( struct colour *c, Enum mode );
   static Code MIBopcode( void ),
               MIBcichar( Code class, Code id );
   static Double MIBreal( Enum type, Prec bits );
   static Long MIBint( Prec p, Enum type ),
               MIBbits ( Prec, Prec* ),
               MIBpointlist( Long *pi, Float *pr, Logical set );
   static Code MIBnextbyte( void );
#else
   void CGMIbin();             /* Main driver routine */
   static Code MIBmfdesc(),    /* Metafile Descriptor elements */
               MIBpdesc(),     /* Picture Descriptor elements */
               MIBpbody();     /* Picture Body Elements */
   static void MIBtext(),      /* Text Elements */
               MIBexternal(),  /* External Elements */
               MIBstring(),    /* Get a string */
               MIBvdc(),       /* Get a VDC */
               MIBclist(),     /* Get a colour list */
               MIBcolour();    /* Get a colour value */
   static Code MIBopcode(),    /* Get an opcode */
               MIBcichar();    /* Opcode conversion to character code */
   static Double MIBreal();    /* Get a real */
   static Long MIBint(),       /* Get an integer */
               MIBbits (),     /* Get a number of bits */
               MIBpointlist(); /* Get a list of points */
   static Code MIBnextbyte();  /* Get a byte */
#endif

/*  External Routines */

#ifdef PROTO
extern Code CGMframe ( Code ),
            CGMoutput ( Code );
#else
extern Code CGMframe(),
            CGMoutput();
#endif

/* Internal Encoding variables */

static Logical miberror;
static Long mibplen, mibbytes, mibdefbytes;
static Logical mibpart = ZERO;

static char *func="CGMibin", mess[40];

/******************************************************* CGMIbin *****/

#ifdef PROTO
void CGMIbin( void )
#else
void CGMIbin()
#endif
{
   Code c = NONOP;

/* Start processing the Metafile */

   while( NEXTCODE( c ) != EOF )
   {

/* Process the Metafile Descriptor elements */

      c = MIBmfdesc(c);
      if ( cgmEOF ) break;

/* Start processing the Pictures */

      while( NEXTCODE(c) != ENDMF )
      {

/* Process the Picture Descriptor elements */

         c = MIBpdesc(c, MULTIPLE);
         if(cgmfinished) break;

/* Process the Picture Body elements */

         c = MIBpbody(c, MULTIPLE);
         if(cgmfinished) break;
      }
      if(cgmfinished) break;
   }
   return;
}

/******************************************************* MIBmfdesc ***/

#ifdef PROTO
static Code MIBmfdesc( Code code )
#else
static Code MIBmfdesc( code )
Code code;
#endif

/* Metafile Descriptor elements */

{

   while ( NEXTCODE(code) != BEGPIC )
   {
      register Long j, n, *pi = pint+1;
      Code class, id;

      miberror = FALSE;

#ifdef DEBUG
    DMESS "Mfdesc element: %x\n", code);
#endif
      switch(code)
      {
         case BEGMF: /* Begin Metafile */
            cgmstate = MF_DESC;

/* Set the Metafile defaults */

            CGMdefaults (BEGMF, BINARY);

/*  Set Common defaults to binary values where needed */

            cur.col_bits = curibin.col_prec;
            cur.colind_bits = curibin.colind_prec;
            cur.vdcmin = 1.0/(1L<<curibin.vdc_fraction);
            cur.max_rgb = curibin.max_rgb;
            MIBstring(str);
            break;

         case EOF:
            break;

         case ENDMF: /* End Metafile */
            cgmstate = MF_CLOSED;
            break;

         case MFVERSION: /* Metafile version */
            cgmverno = *pint = GETINT(int_prec);
            if ( cgmverno > CGMVERSION )
            {
               miberror = TRUE;
               exit( CGMerror(func, ERR_VERSION, FATAL, NULLSTR) );
            }
            break;

         case MFDESC: /* Metafile Description */
            MIBstring(str);
            break;

         case VDCTYPE: /* VDC Type */
            cur.vdc_type = GETENUM;
            break;

         case INTEGERPREC: /* Integer Precision */
            cur.int_bits = GETINT(int_prec);
            curibin.int_prec = cur.int_bits;
            break;

         case REALPREC: /* Real Precision */
            curibin.real_type = GETENUM;
            curibin.real_whole = GETINT(int_prec);
            curibin.real_fraction = GETINT(int_prec);
            if ( curibin.real_type == FIXED )
            {
               cur.real_bits = curibin.real_whole;
               cur.real_places = -curibin.real_fraction;
            }
            if(curibin.real_fraction < sizeof(long)<<3)
               cur.realmin = (1.0/(1L<< curibin.real_fraction));
            else cur.realmin = pow((double)2,(double)(-curibin.real_fraction));
            if(curibin.real_whole < sizeof(long)<<3)
               cur.max_real = (1L<< curibin.real_whole) - 1;
            else cur.max_real = pow((double)2,(double)curibin.real_whole) - 1;
            cur.min_real =  -cur.max_real;
            break;

         case INDEXPREC: /* Index Precision */
            cur.index_bits = curibin.index_prec = GETINT(int_prec);
            break;

         case COLRPREC: /* Colour Precision */
            cur.col_bits = curibin.col_prec = GETINT(int_prec);
            cur.colprec_flag = TRUE;
            break;

         case COLRINDEXPREC: /* Colour Index Precision */
            cur.colind_bits = curibin.colind_prec = GETINT(int_prec);
            break;

         case MAXCOLRINDEX: /* Maximum Colour Index */
            cur.max_colind = MIBint(curibin.colind_prec,UNSIGNED);
            break;

         case COLRVALUEEXT: /* Colour value extent */
            MIBcolour(&cur.min_rgb, DIRECT);
            MIBcolour(&cur.max_rgb, DIRECT);
            curibin.min_rgb = cur.min_rgb;
            curibin.max_rgb = cur.max_rgb;
            cur.colval_flag = TRUE;
            break;

         case MFELEMLIST: /* Metafile element List */
            cgmstate = MF_ELEMLIST;
            *pint = GETINT(int_prec);
            for (n = 0; n < *pint; n++ )
            {
               class = GETINT(index_prec);
               id = GETINT(index_prec);
               if ( class < ZERO ) *pi++ = id;
               else *pi++ = MIBcichar(class,id);
            }
            cgmstate = MF_DESC;
            break;

         case BEGMFDEFAULTS: /* Begin Metafile defaults Replacement */
            mibdefbytes = mibbytes;
            mibbytes = ZERO;
            cgmstate = MF_DEFAULTS;
            (void) CGMoutput ( BEGMFDEFAULTS );
            while ( TRUE )
            {
               code = MIBopcode();
               mibdefbytes -= mibbytes + 2;
               if ( mibdefbytes < 0 || code == EOF ) break;

               if ( code <= BACKCOLR ) (void) MIBpdesc(code, SINGLE);
               else                    (void) MIBpbody(code, SINGLE);
               if( cgmfinished ) break;
               if ( ! mibdefbytes ) break;
            }
            code = ENDMFDEFAULTS;
            cgmstate = MF_DESC;
            break;

         case FONTLIST: /* Font List */
            j = ZERO;
            while( mibbytes > 1 )
            {
               MIBstring(&str[j]);
               j += strlen(&str[j]) + 1;
               *pi++ = j;
            }
            *pint = j;
#ifdef FONTSYSTEM
            FNTflist(pi,pint,str,&cgmfonts);
#endif
            break;

         case CHARSETLIST: /* Character set list */
            j = ZERO;
            while( mibbytes > 1 )
            {
               *pi++ = GETENUM;
               MIBstring(&str[j]);
               j += strlen(&str[j]) + 1;
               *pi++ = j;
            }
            *pint = j;
#ifdef FONTSYSTEM
            FNTclist(pi,pint,str,&cgmcsets);
#endif
            break;

         case CHARCODING: /* Character coding Announcer */
            *pint = GETENUM;
            break;

        default :
            MIBexternal (code);
            break;
      }

/*  Clear any remaining spaces */
      while ( mibbytes > 0 ) (void) MIBnextbyte();
      if ( code = ( miberror ? 0 : CGMframe(code) ) ) break;
   }

   return (code);
}

/******************************************************* MIBpdesc ****/

#ifdef PROTO
static Code MIBpdesc( Code code, Logical single)
#else
static Code MIBpdesc( code, single )
Code code;
Logical single;
#endif

/* Picture Descriptor elements */

{

   while ( NEXTCODE(code) != BEGPICBODY )
   {
      miberror = FALSE;

#ifdef DEBUG
    DMESS "Picture Desc element: %x\n", code);
#endif
      switch(code)
      {
         case BEGPIC:    /* Begin Picture */
            MIBstring(str);

/* Set the Picture defaults */

            if ( cgmstate == MF_DESC )
               CGMdefaults (MFDESC, BINARY);
            else
               CGMdefaults (ENDPIC, BINARY);
            cgmstate = PIC_DESC;
            break;

         case EOF:
            single = TRUE;
#ifdef DEBUG
   DMESS "End of file reached\n");
#endif
            break;

         case ENDPIC:
            cgmstate = PIC_CLOSED;
            return(code);

         case SCALEMODE: /* Scaling Mode */
            cur.scale_mode = GETENUM;
            if ( cur.scale_mode == METRIC )
               cur.scale_factor = MIBreal( FLOATING, cur.real_bits);
            break;

         case COLRMODE: /* Colour Selection Mode */
            cur.color_mode = GETENUM;
            break;

         case LINEWIDTHMODE: /* Line width Specification */
            cur.linewidth_mode = GETENUM;
            break;

         case MARKERSIZEMODE: /* Marker size Specification */
            cur.markersize_mode = GETENUM;
            break;

         case EDGEWIDTHMODE: /* Edge width Specification */
            cur.edgewidth_mode = GETENUM;
            break;

         case VDCEXT: /* VDC Extent */
            if(cur.vdc_type == REAL)
            {
               cur.vdc_extent.a.x.real = GETVDC;
               cur.vdc_extent.a.y.real = GETVDC;
               cur.vdc_extent.b.x.real = GETVDC;
               cur.vdc_extent.b.y.real = GETVDC;
            }
            else
            {
               cur.vdc_extent.a.x.intr = GETINT(vdcint_prec);
               cur.vdc_extent.a.y.intr = GETINT(vdcint_prec);
               cur.vdc_extent.b.x.intr = GETINT(vdcint_prec);
               cur.vdc_extent.b.y.intr = GETINT(vdcint_prec);
            }
            break;

         case BACKCOLR: /* Background Colour */
            MIBcolour(&cur.back, DIRECT);
            break;

         default :
            MIBexternal (code);
            break;
      }

/*  Clear any remaining spaces */
      while ( mibbytes > 0 ) (void) MIBnextbyte();
      code = ( miberror ? ZERO : CGMframe(code) );
      if ( single || cgmfinished ) break;
   }

   return (code);
}

/******************************************************* MIBpbody ****/

#ifdef PROTO
static Code MIBpbody( Code code, Logical single)
#else
static Code MIBpbody( code, single )
Code code;
Logical single;
#endif

/* Picture Body elements */

{
   Code c;
   static Logical first = TRUE;

   while ( NEXTCODE(code) != ENDMF )
   {
      register Long i, k, *pi = pint+1;
      static Long nx, ny;
      static Prec locprec;

      Float *pr = preal;
      struct colour col;

      c = NONOP;
      miberror = FALSE;

#ifdef DEBUG
    DMESS "Picture Body element: %x\n", code);
#endif
      switch(code)
      {
         case BEGPICBODY:     /*  Begin Picture Body */
            cgmstate = PIC_OPEN;
            CGMdefaults(VDCEXT, BINARY);
            break;

         case EOF:
            single = TRUE;
            break;

         case ENDPIC:         /*  End Picture */
            cgmstate = PIC_CLOSED;
            single = TRUE;    /*  Forces exit */
            break;

         case LINE:         /*  Polyline   */
         case DISJTLINE:         /*  Disjoint Polyline  */
         case MARKER:         /*  Polymarker  */
            *pint = MIBpointlist( pi, pr, NOSET);
            if ( *pint < ZERO ) c = code;
            break;

         case POLYGON:         /*  Polygon   */
         case POLYGONSET:         /*  Polygon Set  */
            *pint = MIBpointlist( pi, pr, (Logical)(code == POLYGONSET) );
            if ( *pint < ZERO ) c = code;
            break;

         case APNDTEXT:/* Append Text */
            if ( cgmstate != TEXT_OPEN )
            {
               miberror = TRUE;
               (void) CGMerror(func, ERR_APNDTXT, ERROR, mess);
               break;
            }
            MIBtext (code);
            break;

         case RESTRTEXT: /* Restricted Text */
            if ( cgmstate == TEXT_OPEN )
            {
               miberror = TRUE;
               (void) CGMerror(func, ERR_NOTAPNDTXT, ERROR, mess);
               break;
            }
            MIBvdc((Int)2, pi, pr);
            pi += 2; pr += 2;

         case TEXT: /* Text */
            MIBvdc((Int)2, pi, pr);
            *pint = GETENUM;
            MIBstring(str);
            if ( *pint ) cgmstate = PIC_OPEN;
            else         cgmstate = TEXT_OPEN;
            break;

         case CELLARRAY: /* Cell Array */
         {
            static Long nnx, nny, rowlen;
            static Enum ctype;
            Long *pmax = pimax - 1;

            if ( first )
            {
               MIBvdc((Int)6, pi, pr);
               pi += 6;
/*  Cell array sizes */
               *pi++ = nnx = GETINT(int_prec);
               *pi++ = nny = GETINT(int_prec);
/*  Local Precision */
               *pi++ = locprec = GETINT(int_prec);
/*  Colour list type */
               ctype = GETENUM;
               rowlen = ( cur.color_mode == DIRECT ? 3*nnx : nnx );
#ifdef FIXED_BUFFER
               if ( pi + rowlen > pmax )
               {
                  miberror = TRUE;
                  (void) CGMerror(func, ERR_BIGCELL, ERROR, mess);
                  break;
               }
#else
               pi = CGMialloc ( pi, (long) rowlen*nny );
               pmax = pimax;
#endif
            }

#ifdef DEBUG
  DMESS "\nCell array: %d x %d prec %d\n", nnx, nny, locprec);
  DMESS " Pointers: pi %x pmax %x len: %d (%d)\n", pi, pmax, pmax-pi, rowlen);
#endif

/*  Input a row at a time as rows start on a word boundary */

            *pint = nnx*nny;
            for ( i = 0; i < nny; i++ )
            {
/*  Communications area can not take row */
               if ( pi + rowlen > pmax )
               {
#ifdef FIXED_BUFFER
                  c = code;
                  *pint = - nnx * i;
                  nny -= i;
#ifdef DEBUG
  DMESS " New Buffer line: %d length %d\n", i, *pint);
  DMESS " Pointers: pi %x pmax %x len: %d (%d)\n", pi, pmax, pmax-pi, rowlen);
#endif
                  break;
#endif
               }
               MIBclist( pi, nnx, locprec, ctype );
               if ( cgmEOF ) break;
               pi += rowlen;
            }
            first = ( *pint > ZERO );
            if ( *pint < ZERO ) c = code;
            break;
         }

         case GDP: /* Generalised Drawing Primitive */
            *pi++ = GETINT(int_prec);
            *pint = GETINT(int_prec);
            MIBvdc( (Int)(2*(*pint)), pi, pr );
            MIBstring(str);
            break;

         case RECT: /* Rectangle */
            MIBvdc((Int)4, pi, pr);
            break;

         case CIRCLE: /* Circle */
            MIBvdc((Int)3, pi, pr);
            break;

         case ARC3PT: /* Circular Arc 3 point */
            MIBvdc((Int)6, pi, pr);
            break;

         case ARC3PTCLOSE: /* Circular Arc 3 point close */
            MIBvdc((Int)6, pi, pr);
            *(pi+6) = GETENUM;
            break;

         case ARCCTR: /* Circle Arc centre */
            MIBvdc((Int)7, pi, pr);
            break;

         case ARCCTRCLOSE: /* Circle Arc centre close */
            MIBvdc((Int)7, pi, pr);
            *(pi+7) = GETENUM;
            break;

         case ELLIPSE: /* Ellipse */
            MIBvdc((Int)6, pi, pr);
            break;

         case ELLIPARC: /* Elliptical Arc */
            MIBvdc((Int)10, pi, pr);
            break;

         case ELLIPARCCLOSE: /* Elliptical Arc close */
            MIBvdc((Int)10, pi, pr);
            *(pi+10) = GETENUM;
            break;

/*  Attributes */

         case LINEINDEX: /* Line Bundle index */
            curatt.line_ind = GETINT(index_prec);
            break;

         case LINETYPE: /* Line Type */
            curatt.line_type = GETINT(index_prec);
            break;

         case LINEWIDTH: /* Line Width */
            if ( cur.linewidth_mode == SCALED )
                        curatt.line_width.real = GETREAL;
            else
            {
               if ( cur.vdc_type == REAL )
                  curatt.line_width.real = GETVDC;
               else
                  curatt.line_width.intr = GETINT(vdcint_prec);
            }
            curatt.linewidth_flag = TRUE;
            break;

         case LINECOLR: /* Line Colour */
            MIBcolour(&curatt.line, cur.color_mode);
            break;

         case MARKERINDEX: /* Marker Bundle index */
            curatt.mark_ind = GETINT(index_prec);
            break;

         case MARKERTYPE: /* Marker Type */
            curatt.mark_type = GETINT(index_prec);
            break;

         case MARKERSIZE: /* Marker Size */
            if ( cur.markersize_mode == SCALED )
               curatt.mark_size.real = GETREAL;
            else
            {
               if ( cur.vdc_type == REAL )
                  curatt.mark_size.real = GETVDC;
               else
                  curatt.mark_size.intr = GETINT(vdcint_prec);
            }
            curatt.markersize_flag = TRUE;
            break;

         case MARKERCOLR: /* Marker Colour */
            MIBcolour(&curatt.marker, cur.color_mode);
            break;

         case CHARORI: /* Character Orientation */
            if(cur.vdc_type == REAL)
            {
               curatt.char_up.x.real = GETVDC;
               curatt.char_up.y.real = GETVDC;
               curatt.char_base.x.real = GETVDC;
               curatt.char_base.y.real = GETVDC;
            }
            else
            {
               curatt.char_up.x.intr = GETINT(vdcint_prec);
               curatt.char_up.y.intr = GETINT(vdcint_prec);
               curatt.char_base.x.intr = GETINT(vdcint_prec);
               curatt.char_base.y.intr = GETINT(vdcint_prec);
            }
            break;

         case TEXTALIGN: /* Text Alignment */
            curatt.text_halign = GETENUM;
            curatt.text_valign = GETENUM;
            curatt.text_hcont  = GETREAL;
            curatt.text_vcont  = GETREAL;
            break;

         case FILLINDEX: /* Fill Bundle index */
            curatt.fill_ind = GETINT(index_prec);
            break;

         case INTSTYLE: /* Interior Style */
            curatt.int_style = GETENUM;
            break;

         case FILLCOLR: /* Fill Colour */
            MIBcolour(&curatt.fill, cur.color_mode);
            break;

         case HATCHINDEX: /* Hatch Index */
            curatt.hatch_ind = GETINT(index_prec);
            break;

         case PATINDEX: /* Pattern Index */
            curatt.pat_ind = GETINT(index_prec);
            break;

         case EDGEINDEX: /* Edge Bundle index */
            curatt.edge_ind = GETINT(index_prec);
            break;

         case EDGETYPE: /* Edge Type */
            curatt.edge_type = GETINT(index_prec);
            break;

         case EDGEWIDTH: /* Edge Width */
            if ( cur.edgewidth_mode == SCALED )
                  curatt.edge_width.real = GETREAL;

            else
            {
               if ( cur.vdc_type == REAL )
                  curatt.edge_width.real = GETVDC;
               else
                  curatt.edge_width.intr = GETINT(vdcint_prec);
            }
            curatt.edgewidth_flag = TRUE;
            break;

         case EDGECOLR: /* Edge Colour */
            MIBcolour(&curatt.edge, cur.color_mode);
            break;

         case EDGEVIS: /* Edge Visibility */
            curatt.edge_vis = GETENUM;
            break;

         case FILLREFPT: /* Fill Reference Point */
            if(cur.vdc_type == REAL)
            {
               curatt.fill_ref.x.real = GETVDC;
               curatt.fill_ref.y.real = GETVDC;
            }
            else
            {
               curatt.fill_ref.x.intr = GETINT(vdcint_prec);
               curatt.fill_ref.y.intr = GETINT(vdcint_prec);
            }
            break;

         case PATTABLE: /* Pattern Table */
            if ( first )
            {
               *pi++ = GETINT(index_prec);  /*  Pattern Index */
               *pi++ = nx = GETINT(int_prec);
               *pi++ = ny = GETINT(int_prec);
               *pint = nx * ny;            /*  no of elements */
               *pi++ = locprec = GETINT(int_prec);
            }

/*  Get as a single Packed list   */
            MIBclist( pi, *pint, locprec, PACKED);
            first = ( *pint > ZERO );
            break;

         case PATSIZE: /* Pattern Size */
            if(cur.vdc_type == REAL)
            {
               curatt.pat_size.a.x.real = GETVDC;
               curatt.pat_size.a.y.real = GETVDC;
               curatt.pat_size.b.x.real = GETVDC;
               curatt.pat_size.b.y.real = GETVDC;
            }
            else
            {
               curatt.pat_size.a.x.intr = GETINT(vdcint_prec);
               curatt.pat_size.a.y.intr = GETINT(vdcint_prec);
               curatt.pat_size.b.x.intr = GETINT(vdcint_prec);
               curatt.pat_size.b.y.intr = GETINT(vdcint_prec);
            }
            break;

         case COLRTABLE: /* Colour Table */
            if ( first )
            {
               *pi++ = MIBint(curibin.colind_prec,UNSIGNED);
            }
            k = mibbytes*BYTE/(3*curibin.col_prec);
            for(i = 0; i < k; i++)
            {
               MIBcolour(&col, DIRECT);
               if ( cgmEOF ) break;
               *pi++ = col.red;
               *pi++ = col.green;
               *pi++ = col.blue;
            }
            *pint = k;
            break;

         case ASF: /* Aspect source flags */
            for(i = 0; mibbytes > 1; i++)
            {
               *pi++ = GETENUM;
               *pi++ = GETENUM;
            }
            *pint = i;
            CGMsetasf( pint );   /* set current values for ASFs */
            break;

/*  Control Elements */

         case VDCINTEGERPREC: /* VDC Integer Precision */
            curibin.vdcint_prec = GETINT(int_prec);
            cur.vdcint_bits = curibin.vdcint_prec;
            break;

         case VDCREALPREC: /* VDC Real Precision */
            curibin.vdc_type = GETENUM;
            curibin.vdc_whole = GETINT(int_prec);
            curibin.vdc_fraction = GETINT(int_prec);
            if ( curibin.real_type == FIXED )
            {
               cur.vdc_bits = curibin.vdc_whole;
               cur.vdc_places = -curibin.vdc_fraction;
            }
            if(curibin.vdc_fraction < sizeof(long)<<3)
               cur.vdcmin = (1.0/(1L<< curibin.vdc_fraction));
            else cur.vdcmin = pow((double)2, (double)(-curibin.vdc_fraction));
            if(curibin.vdc_whole < sizeof(long)<<3)
               cur.max_vdc = (1L<< curibin.vdc_whole) - 1;
            else cur.max_vdc = pow((double)2, (double) curibin.vdc_whole) - 1;
            cur.min_vdc =  -cur.max_vdc;
            break;

         case CLIP: /* Clip Indicator */
            cur.clip_ind = GETENUM;
            break;

         case CLIPRECT: /* Clip Rectangle */
            if(cur.vdc_type == REAL)
            {
               cur.clip_rect.a.x.real = GETVDC;
               cur.clip_rect.a.y.real = GETVDC;
               cur.clip_rect.b.x.real = GETVDC;
               cur.clip_rect.b.y.real = GETVDC;
            }
            else
            {
               cur.clip_rect.a.x.intr = GETINT(vdcint_prec);
               cur.clip_rect.a.y.intr = GETINT(vdcint_prec);
               cur.clip_rect.b.x.intr = GETINT(vdcint_prec);
               cur.clip_rect.b.y.intr = GETINT(vdcint_prec);
            }
            curatt.cliprect_flag = TRUE;
            break;

         default:  /* code not yet found try Text */
            MIBtext (code);
            break;
      }

/*  Clear any remaining spaces if not continued */
      if ( *pint >= 0 ) while ( mibbytes > 0 ) (void) MIBnextbyte();
      if ( code = ( miberror ? 0 : CGMframe(code) ) )
      {
         mibbytes = -1;
      }
      code = ( code > 0 ? code : c );

      if ( single || cgmfinished ) break;
   }

   return (code);
}

/******************************************************* MIBtext *******/

#ifdef PROTO
static void MIBtext( Code code )
#else
static void MIBtext( code )
Code code;
#endif
{
   switch (code)
   {
      case APNDTEXT:/* Append Text */
         if ( cgmstate != TEXT_OPEN )
         {
            miberror = TRUE;
            (void) CGMerror(func, ERR_APNDTXT, ERROR, mess);
            break;
         }
         *pint = GETENUM;
         MIBstring(str);
         cgmstate = ( *pint ? PIC_OPEN : TEXT_OPEN );
         break;

      case TEXTINDEX: /* Text Bundle index */
         curatt.text_ind = GETINT(index_prec);
         break;

      case TEXTFONTINDEX: /* Text Font Index */
         curatt.text_font = GETINT(index_prec);
         break;

      case TEXTPREC: /* Text Precision */
         curatt.text_prec = GETENUM;
         break;

      case CHAREXPAN: /* Character Expansion Factor */
         curatt.char_exp = GETREAL;
         break;

      case CHARSPACE: /* Character Spacing */
         curatt.char_space = GETREAL;
         break;

      case TEXTCOLR: /* Text Colour */
         MIBcolour(&curatt.text, cur.color_mode);
         break;

      case CHARHEIGHT: /* Character Height */
         if(cur.vdc_type == REAL)
              curatt.char_height.real = GETVDC;
         else curatt.char_height.intr = GETINT(vdcint_prec);
         curatt.charheight_flag = TRUE;
         break;

      case TEXTPATH: /* Text Path */
         curatt.text_path = GETENUM;
         break;

      case CHARSETINDEX: /* Character Set Index */
         curatt.char_set = GETINT(index_prec);
         break;

      case ALTCHARSETINDEX: /* Alternative Character Set Index */
         curatt.altchar_set = GETINT(index_prec);
         break;

      case AUXCOLR: /* Auxiliary Colour */
         MIBcolour(&cur.aux, cur.color_mode);
         break;

      case TRANSPARENCY: /* Transparency */
         cur.transparency = GETENUM;
         break;

      case ESCAPE: /* Escape */
         *pint = GETINT(int_prec);
         MIBstring(str);
         break;

      default:    /* Not yet found try External elemants */
         MIBexternal (code);
         break;
   }

   return;
}

/******************************************************* MIBexternal ***/

#ifdef PROTO
static void MIBexternal( Code code )
#else
static void MIBexternal( code )
Code code;
#endif
{
   switch (code)
   {
      case MESSAGE: /* Message */
         *pint = GETENUM;
         MIBstring(str);
         break;

      case APPLDATA: /* Application Data */
         *pint = GETINT(int_prec);
         MIBstring(str);
         break;

      case NONOP:  /* Non-Op code 0 */
         break;

      default:
         if ( miberror )  /* if already an error give up */
         {
            (void) CGMerror(func, ERR_MANYERR, FATAL, NULLSTR);
            cgmEOF = TRUE;
         }
         else
         {
            miberror = TRUE;
            if ( cgmstate == MF_CLOSED ) break;
            sprintf( mess, "(%4x)", code );
            switch ( cgmstate )
            {
               case MF_DESC:
                  (void) strcat(mess, " in Metafile Description");
                  break;
               case MF_ELEMLIST:
                  (void) strcat(mess, " in MF Element list");
                  break;
               case MF_DEFAULTS:
                  (void) strcat(mess, " in MF Defaults Replacement");
                  break;
               case PIC_DESC:
                  (void) strcat(mess, " in Picture Description");
                  break;
               case PIC_OPEN:
                  (void) strcat(mess, " in Picture Body");
                  break;
               case PIC_CLOSED:
                  (void) strcat(mess, " while Picture Closed");
                  break;
               case TEXT_OPEN:
                  (void) strcat(mess, " in Partial Text");
                  break;
               default:
                  (void) sprintf(mess, " in Unknown State");
                  break;
            }
            (void) CGMerror(func, ERR_INVELEM, ERROR, mess);
         }
         break;
   }

   return;
}

/******************************************************* MIBclist ****/

#ifdef PROTO
static void MIBclist( Long *pi, Long nx, Prec prec, Enum type )
#else
static void MIBclist( pi, nx, prec, type )
Long *pi, nx;
Prec prec;
Enum type;
#endif

/* Get a row of a cell array */

{
   register Long j, k, run;
   unsigned Long col, red, green, blue;
   Prec bit;

#ifdef DEBUG
   DMESS "Colour list: %d cells prec: %d type: %d (0x%x)\n",
            nx, prec, type, pi );
#endif

   if ( !prec ) prec = curibin.col_prec;

   for ( j = 0, bit = 0; j < nx; )
   {
      run = ( type == RUNLENGTH ? MIBbits(curibin.int_prec, &bit) : 1 );

      if ( cur.color_mode == DIRECT )
      {
         red   = MIBbits( prec, &bit );
         green = MIBbits( prec, &bit );
         blue  = MIBbits( prec, &bit );
#ifdef DEBUG
   DMESS "RGB Colour %d: %d %d %d", j, red, green, blue );
   if ( type == RUNLENGTH ) DMESS " (run: %d)", run);
   DMESS "\n");
#endif
      }
      else
      {
         col = MIBbits( prec, &bit );
#ifdef DEBUG
   DMESS "Indexed Colour %d: %d\n", j, col );
   if ( type == RUNLENGTH ) DMESS " (run: %d)", run);
   DMESS "\n");
#endif
      }
      if ( cgmEOF ) return;

      j += run;
      if ( j > nx ) run = j - nx;
      for ( k = 0; k < run ; k++ )
      {
         if ( cur.color_mode == DIRECT )
         {
            *pi++ = red; *pi++ = green; *pi++ = blue;
         }
         else
         {
            *pi++ = col;
         }
      }
   }

   return;
}

/******************************************************* MIBbits *****/

#ifdef PROTO
static Long MIBbits ( Prec prec, Prec *bit )
#else
static Long MIBbits ( prec, bit )

/* Get next 'prec' bits from input */

Prec prec, *bit;
#endif

{
   static Posint oneword, wordmask;
   Long col = 0, k;

#ifdef DEBUG2
   DMESS " Get bits: %d bit: %d", prec, *bit );
#endif

   while ( prec > 0 )
   {
      k = ( prec >= WORD ? WORD : prec );
      if ( *bit < k ) k = *bit;
      if ( ! *bit )
      {
         oneword = MIBint( WORD, UNSIGNED);
#ifdef DEBUG2
   DMESS " *(0x%04x)", oneword );
#endif
         *bit = WORD;
         wordmask = (Posint)0xffff;
      }
      *bit -= k;
      col <<= k;
      col |= oneword >> *bit;
      wordmask >>= k;
      oneword &= wordmask;
#ifdef DEBUG2
   DMESS "\n   prec: %d, col: %d, bit: %d k: %d\n", prec, col, *bit, k );
#endif
      prec -= k;
   }

   return ( col );
}

/******************************************************* MIBstring ***/

#ifdef PROTO
static void MIBstring( char *s1 )
#else
static void MIBstring(s1)
char *s1;
#endif

/* Get a string */

{
   register Code c;
   register Long len;
   unsigned Long n;
   register char *s=s1;

   if ( mibplen )
   {
      len = MIBnextbyte();
      if ( len == (long)0xff )
      {
         len = 0L;
         do
         {
            n = MIBint ( WORD, UNSIGNED );
            len <<= 16;
            len += n & (unsigned Long)0x7fff;
         } while ( n & (unsigned Long)0x8000 );
      }

      for ( ; len; len-- )
      {
         if( !mibbytes ) break;
         c = MIBnextbyte();
#ifdef EBCDIC
         *s++= cgmebcdic[c];
#else
         *s++= c;
#endif
      }
  }
  *s++= '\0';

#ifdef DEBUG1
   DMESS " String: %s\n", s1);
#endif
   return;
}

/******************************************************* MIBpointlist */

#ifdef PROTO
static Long MIBpointlist( Long *pi, Float *pr, Logical set )
#else
static Long MIBpointlist( pi, pr, set )
Long *pi;
Float *pr;
Logical set;
#endif

/* Get a list of points */

{
   register Long i;
   static Logical first = TRUE;
   Long *pmax;
   Float *pmaxreal;

   if ( first )
   {
/*  Set maximum pointer values */
      pmax = pimax - 3;
      pmaxreal = prmax - 2;
   }

   for ( i = 0; mibbytes ; i++ )
   {
      if( cur.vdc_type == REAL )
      {
         if ( pr > pmaxreal )
#ifdef FIXED_BUFFER
         {
            first = FALSE;
            return (-i);
         }
#else
         {
            pr = CGMralloc ( pr, 0L );
            pmaxreal = prmax - 2;
         }
#endif
         *pr++ = GETVDC;
         *pr++ = GETVDC;
      }
      else
      {
         if ( pi > pmax )
#ifdef FIXED_BUFFER
         {
            first = FALSE;
            return (-i);
         }
#else
         {
            pi = CGMialloc ( pi, 0L );
            pmax = pimax - 3;
         }
#endif
         *pi++ = GETINT(vdcint_prec);
         *pi++ = GETINT(vdcint_prec);
      }
      if( set ) *pi++ = GETENUM;
      if ( cgmEOF ) break;
   }
   first = TRUE;
   return ( i );
}

/******************************************************* MIBvdc ******/

#ifdef PROTO
static void MIBvdc( Int n, Long *pi, Float *pr )
#else
static void MIBvdc(n, pi, pr )
Int n;
Long *pi;
Float *pr;
#endif

/* Get n VDC values starting at array index i */

{
   register Long j;

   for ( j = 0; j < n ; j++ )
   {
      if ( cur.vdc_type == REAL ) *pr++ = GETVDC;
      else *pi++ = GETINT(vdcint_prec);
      if ( cgmEOF ) break;
   }
   return;
}

/******************************************************* MIBcolour ***/

#ifdef PROTO
static void MIBcolour( struct colour *c, Enum colour_mode )
#else
static void MIBcolour( c, colour_mode )
struct colour *c;
Enum colour_mode;
#endif

/* Get a colour value */

{
   if ( colour_mode == DIRECT )
   {
     c->red = MIBint(curibin.col_prec,UNSIGNED);
     c->green = MIBint(curibin.col_prec,UNSIGNED);
     c->blue = MIBint(curibin.col_prec,UNSIGNED);
   }
   else
     c->index = MIBint(curibin.colind_prec,UNSIGNED);

   return;
}

/******************************************************* MIBopcode ***/

#ifdef PROTO
static Code MIBopcode( void )
#else
static Code MIBopcode()
#endif

/* Get an opcode */

{
   Code c;
   Code byte1 = ZERO, byte2 = ZERO;
   Code oneword;
   static Code class, id;

   mibbytes--;  /*  set to -1 */

#ifdef DEBUG
   DMESS "\n");
#endif

/*  get next two bytes */

   byte1 = MIBnextbyte();
   byte2 = MIBnextbyte();
   if ( cgmEOF ) return ( (Code) EOF );

   oneword = (byte1<<BYTE) + byte2;
   class = (byte1 & 0xf0) >> 4;
   id = (oneword & 0x0fe0) >> 5;
   mibplen = (byte2 & 0x1f);

   if ( mibplen == 0x1f )  /* Parameter length greater than 30 */
   {
      if ( cgmstate == MF_DEFAULTS ) mibdefbytes -= 2;
      byte1 = MIBnextbyte();
      if ( cgmEOF ) return ( (Code) EOF );
      mibplen = (byte1 & 0x7f)<<BYTE;
      mibplen += (long)MIBnextbyte();
      mibpart = byte1 & 0x80;
   }
   mibbytes = mibplen;
   if ( mibbytes & 1L ) mibbytes++;

   c = MIBcichar(class, id);

#ifdef DEBUG
   DMESS "Opcode : %x class: %d id: %d", c, class, id);
   if ( mibpart ) DMESS " (Partitioned)");
   DMESS " Number of Bytes: %d (%d)\n",mibplen, mibbytes);
#endif

   return(c);
}

/******************************************************* MIBcichar ***/

#ifdef PROTO
static Code MIBcichar( Code class, Code id )
#else
static Code MIBcichar( class, id )
Code class, id;
#endif

/* Convert Binary opcode to Character opcode */

{
   Code c = ZERO;

   switch(class)
   {
      case 0:     /* Delimiter elements */
         if ( ! id ) return(0);        /* Non Op */
         if ( id <= 5 ) c = BEGMF + id;
         break;
      case 1:    /*MF Descriptor elements */
         if ( !id ) break;
         if ( id <= 12 ) c = MFVERSION + id;
         else if ( id <= 15 ) c = MFVERSION + id + 1;
         break;
      case 2:    /* Picture Descriptor elements */
         if ( id > 0 && id <= 7 ) c = SCALEMODE + id;
         break;
      case 3:    /* Control Elements */
         if ( id > 0 && id <= 6 ) c = VDCINTEGERPREC + id;
         break;
      case 4:    /* Graphics primitives */
         if ( !id ) break;
         if ( id <= 11 ) c = LINE + id;
         else if ( id <= 19 ) c = CIRCLE + id - 11;
         break;
      case 5:    /*  Attributes */
         if ( !id ) break;
         if ( id <= BYTE ) c = LINEINDEX + id;
         else if ( id <= 20 ) c = TEXTINDEX + id - BYTE;
         else if ( id <= 33 ) c = FILLINDEX + id - 20;
         else if ( id <= 35 ) c = COLRTABLE + id - 33;
         break;
      case 6:    /*  Escape element */
         if ( id == 1 ) c = ESCAPE + id;
         break;
      case 7:    /* External Elements */
         if ( id > 0 && id <= 2 ) c = MESSAGE + id;
         break;
   }

   if ( c ) c--;
   else if ( cgmstate != MF_CLOSED )
   {
      miberror = TRUE;
      (void) sprintf(mess,"(class: %d id: %d)", class, id);
      (void) CGMerror(func, ERR_ELEMENT, ERROR, mess);
   }

   return(c);
}

/******************************************************* MIBreal *****/

#ifdef PROTO
static Double MIBreal( Enum real_type, Prec real_prec )
#else
static Double MIBreal(real_type,real_prec)
Enum real_type; /* Fixed or Floating point */
Prec real_prec; /* From cur.(real/vdc)_bits, ie half the TOTAL number of bits */
#endif

/* Get a real value */

{
   static Logical first = TRUE;
   static Double ptab[2*NPOWER+1];
   register Double num, *power2=ptab+NPOWER;
   register Long whole, mantissa;
   register Posint fract;
   register int byte, sign, exponent;

   if ( first )
   {
/*  Set up table for powers of two */
      *(power2) = 1.0;
      for ( exponent = 1, num = 1.0; exponent <= NPOWER; exponent++)
      {
         num *= 2.0;
         *(power2+exponent) = num;
         *(power2-exponent) = 1.0/num;
      }
      first = FALSE;
   }

   byte = MIBnextbyte();
   sign = byte & 0x80;
   if ( real_type == FIXED )
   {
      whole = (byte & 0x7F) << BYTE;
      if ( real_prec == 32 )
      {
/*  Fixed point 64-bit  */
         whole += (Long)MIBnextbyte(); whole <<= BYTE;
         whole += (Long)MIBnextbyte(); whole <<= BYTE;
         whole += (Long)MIBnextbyte();
         fract  = (Posint)MIBnextbyte() << BYTE;
         fract += (Posint)MIBnextbyte(); fract <<= BYTE;
         fract += (Posint)MIBnextbyte(); fract <<= BYTE;
         fract += (Posint)MIBnextbyte();
         if(sign) num = (Double)whole - (Double)fract/TWO32;
         else     num = (Double)whole + (Double)fract/TWO32;
      }
      else
      {
/*  Fixed point 32-bit  */
         whole += (Long)MIBnextbyte();
         fract  = (Posint)MIBnextbyte() << BYTE;
         fract += (Posint)MIBnextbyte();
         if(sign) num = (Double)whole - (Double)fract/TWO16;
         else     num = (Double)whole + (Double)fract/TWO16;
      }
   }
   else
   {
      if ( real_prec == 64 )
      {
/*  IEEE Floating point 64-bit */
         exponent = (byte & 0x7F) << 4;
         byte = MIBnextbyte();
         exponent += (byte & 0xF0) >> 4;
         mantissa = (Long)(byte & 0x0F) << BYTE;
         mantissa += (Long)MIBnextbyte();  mantissa <<= BYTE;
         mantissa += (Long)MIBnextbyte();  mantissa <<= BYTE;
         mantissa += (Long)MIBnextbyte();  mantissa <<= BYTE;
         mantissa += (Long)MIBnextbyte();  mantissa <<= BYTE;
         mantissa += (Long)MIBnextbyte();  mantissa <<= BYTE;
         mantissa += (Long)MIBnextbyte();
         if ( exponent )
         {
            num = 1.0 + (Double)mantissa / TWO52; exponent -= 1023;
         }
         else if (mantissa)
         {
            num = (Double)mantissa / TWO52; exponent = -1022;
         }
         else
         {
            exponent = 0; num = 0;
         }
      }
      else
      {
/*  IEEE Floating point 32-bit */
         exponent = (byte & 0x7F) << 1;
         byte = MIBnextbyte();
         exponent += (byte & 0x80)>>7;
         mantissa  = (Long)(byte & 0x7F) << BYTE;
         mantissa += (Long)MIBnextbyte();  mantissa <<= BYTE;
         mantissa += (Long)MIBnextbyte();
         if ( exponent )
         {
            num = 1.0 + (Double)mantissa / TWO23; exponent -= 127;
         }
         else if (mantissa)
         {
            num = (Double)mantissa / TWO23; exponent = -126;
         }
         else
         {
            exponent = 0; num = 0.0;
         }
      }

      while ( exponent > NPOWER )
      {
         num *= *(power2+NPOWER);
         exponent -= NPOWER;
      }
      while ( exponent < -NPOWER )
      {
         num *= *(power2-NPOWER);
         exponent += NPOWER;
      }
      num *= *(power2+exponent);
   }

#ifdef DEBUG
   if ( real_type == FIXED )
      DMESS "Fixed point real (%d/%d):", whole, fract);
   else
      DMESS "Floating point real (0x%x/%d):", mantissa, exponent);
   DMESS " %f\n", (sign ? -num : num));
#endif

   return (sign ? -num : num);
}

/******************************************************* MIBint ******/

#ifdef PROTO
static Long MIBint( Prec int_prec, Enum int_type )
#else
static Long MIBint( int_prec, int_type )
Prec int_prec;
Enum int_type;
#endif

/* Get an integer value */

{
   Long num;
   Long bitmask = -1;
   Long bytes,i;
   Long sign = ZERO;

   bytes =  int_prec>>3;
   bitmask <<= (int_prec - 1);
   num = MIBnextbyte();
   if ( int_type == SIGNED )
   {
      sign = num & 0x80;
      num &= 0x7F;
      for ( i = 2; i <= bytes; i++ )
      {
         num <<= BYTE;
         num |= MIBnextbyte();
      }
      if ( sign && num )
          if ( cgmralbin ) num += bitmask;
          else             num |= bitmask;
   }
   else
   {
      for ( i = 2; i <= bytes; i++)
      {
         num <<= BYTE;
         num += MIBnextbyte();
      }
   }
#ifdef DEBUG
   DMESS "Integer : %d (0x%x)\n", num, num );
#endif
   return(num);
}

/******************************************************* MIBnextbyte */

#ifdef PROTO
static Code MIBnextbyte( void )
#else
static Code MIBnextbyte()
#endif

/* Get a byte from the input file */

{
   Code c = 0;

   c = fgetc(cgmi);
   mibbytes--;
   if ( !mibbytes && mibpart )             /* New Partition */
   {
      Code d;

      d = fgetc(cgmi);
      mibplen = (Long)(d & 0x7f)<<BYTE;
      mibpart = (Logical)(d & 0x80);
      mibplen += (Long)fgetc(cgmi);
      mibbytes = mibplen;
      if ( mibbytes & 1L ) mibbytes++;
#ifdef DEBUG1
      DMESS "\n New Partition length: %d\n", mibplen);
#endif
   }
#ifdef DEBUG
   DMESS "(0x%2x,%d)", c, mibbytes);
#endif

   if ( c == EOF )
   {
      mibbytes = 0L;
      cgmEOF = TRUE;
   }

   return(c);
}
