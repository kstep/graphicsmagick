/*   RAL-CGM Interpreter module:  @(#) cgmlib.c  version 3.1
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
 * Description:User callable library for CGM calls
 *
 *  Created:   19 Jun 90   RTP
 *
 *  Modification log
 *
 *   9 Jan 91 RTP  Original version released
 *  11 Feb 91 RTP  Add break statement after case 'B' in CGMout
 *                 Otherwise Binary output also contains Character
 *  15 Mar 91 RTP  Use Enum types for cgmdriver
 *                 Change cgmstate to type Enum
 *   3 Apr 91 RTP  Change cgmofile to extern, correct equality check for
 *                 cgmstate and correct CGMdefaults argument to Code
 *                 Correct prototypes for void functions
 *                 Remove references to cgmonative (use cgmdriver)
 *  14 May 91 RTP  Add ANSI declarations
 *  17 May 91 RTP  Change calloc calls to MALLOC
 *   2 Aug 91 RTP  Add SCCS id
 *  19 Nov 92 RTP  Change arguments to CGMialloc etc
 */

/*  CGM Library calls */

#define CGMLIB_C
#include "cgmlib.h"

/* Driver Routines */

#ifdef PROTO
extern void CGMOchar( FILE*, Code, long*, float*, char* );
extern void CGMOtext( FILE*, Code, long*, float*, char* );
extern void CGMObin( FILE*, Code, long*, float*, char* );

void cgminit ( Enum, char* );
void CGMconvert_points ( int, Point*, long*, float*, Enum* );

#ifdef POSTSCRIPT
extern void CGMOps ( FILE*, Code, long*, float*, char* );
#endif
#ifdef IGL
extern void CGMOigl( Code, long*, float*, char* );
#endif
#ifdef XW
extern void CGMOXw( Code, long*, float*, char* );
#endif

#else /* End ANSI prototypes */

extern void CGMOchar();
extern void CGMOtext();
extern void CGMObin();

void cgminit ();
void CGMconvert_points ();

#ifdef POSTSCRIPT
extern void CGMOps ();
#endif
#ifdef IGL
extern void CGMOigl();
#endif
#ifdef XW
extern void CGMOXw();
#endif
#endif

#ifdef XW
extern char terminal[];
#endif

/*  Local Macro */

#define ERRMESS     (void) fprintf(stderr,
#define OUTMESS     (void) fprintf(stderr,

static char mess[40];
char *cgmofile;

/*  Initialise CGM output */

#ifdef PROTO
void cgminit ( Enum type, char *filename )
#else
void cgminit ( type, filename )
Enum type;
char *filename;
#endif
{

   cgmerr = stderr;
   cgmofile = (char *) MALLOC( NAMELGTH+EXTLGTH+LFILEDEF+3, sizeof(char) );;

#ifdef VAR_BUFFER
   pint = (long *) MALLOC( ARRAY_MAX, sizeof(long) );
   pimax = pint + ARRAY_MAX;
   preal = (float *) MALLOC( ARRAY_MAX, sizeof(float) );
   prmax = preal + ARRAY_MAX;
   str = (char *) MALLOC( STRING_MAX, sizeof(char) );
   strmax = str + STRING_MAX;
   if ( pint == NULL || preal == NULL || str == NULL )
   {
      CGMerror ("cgminit", ERR_NOMEMORY, FATAL, NULLSTR);
   }
#endif

   strcpy(cgmofile, filename);

   switch ( type )
   {
#ifdef EBCDIC
      case NATIVE:
          cgmdriver = NATIVE;
          break;
#endif
      case CHARACTER:
          cgmdriver = CHARACTER;
          break;
      case BINARY:
          cgmdriver = BINARY;
          break;
      case CLEAR_TEXT:
          cgmdriver = CLEAR_TEXT;
          break;
#ifdef IGL
      case IGL:
          cgmdriver = IGL;
          return;
#endif
#ifdef XW
      case XW:
          cgmdriver = XW;
          strcpy ( cgmofile, terminal );
          return;
#endif
#ifdef POSTSCRIPT
      case POSTSCRIPT:
          cgmdriver = POSTSCRIPT;
          break;
#endif
#ifdef HPGL
      case HPGL:
          cgmdriver = HPGL;
          break;
#endif
#ifdef TEK4200
      case TEK4200:
          cgmdriver = TEK4200;
          break;
#endif
      default:
          sprintf(mess,"(type: %d)", type);
          exit (CGMerror ("cgminit", ERR_INVOUT, FATAL, mess) );
   }
   if ( strcmp ( filename, "" ) != 0 )
            cgmo = CGMfopen ( cgmofile, F_WRITE, type);
   else cgmo = stdout;
   return;
}

/********************************************************* CGMout ******/

#ifdef PROTO
void CGMout ( Code c, long *pi, float *pf, char *pc )
#else
void CGMout ( c, pi, pf, pc )

Code c;
long *pi;
float *pf;
char *pc;
#endif

{
   if ( c == NONOP ) return;

#ifdef DEBUG
   OUTMESS "Output code: 0x%x\n", c );
#endif

   switch ( cgmdriver )
   {
      case BINARY:
         CGMObin( cgmo, c, pi, pf, pc );
         break;

#ifdef EBCDIC
      case NATIVE:
#endif
      case CHARACTER:
         CGMOchar( cgmo, c, pi, pf, pc );
         break;

      case CLEAR_TEXT:
         CGMOtext( cgmo, c, pi, pf, pc );
         break;

#ifdef POSTSCRIPT
      case POSTSCRIPT:
         CGMOps( cgmo, c, pi, pf, pc );
         break;
#endif

#ifdef IGL
      case IGL:
         CGMOigl( c, pi, pf, pc );
         break;
#endif
#ifdef XW
      case XW:
         CGMOXw( c, pi, pf, pc );
         break;
#endif

#ifdef HPGL
      case HPGL:
         CGMOhpgl( cgmo, c, pi, pf, pc );
         break;
#endif

#ifdef TEK4200
      case TEK4200:
         CGMOtek( cgmo, c, pi, pf, pc );
         break;
#endif

      default:
         break;
   }

   return;
}
/*********************************************** CGMconvert_points *****/

#ifdef PROTO
void CGMconvert_points ( int num, Point *points, long *pin,
                         float *pfl, Enum *set )
#else
void CGMconvert_points ( num, points, pin, pfl, set )
int num;
Point *points;
long *pin;
float *pfl;
Enum *set;
#endif
{
   register int i;
   long *pi=pin+1;
   float *pr=pfl;

   if ( cur.vdc_type == REAL )
      for ( i=0; i<num; i++ )
      {
         if ( pr+2 > prmax )
         {
#ifdef FIXED_BUFFER
            return;
#else
            pr = CGMralloc ( pr, 0L );
#endif
         }
         *pr++ = points->x;
         *pr++ = points->y;
         points++;
         if ( set != NULL ) *pi++ = *set++;
      }
   else
      for ( i=0; i<num; i++ )
      {
         if ( pi+3 > pimax )
         {
#ifdef FIXED_BUFFER
            return;
#else
            pi = CGMialloc ( pi, 0L );
#endif
         }
         *pi++ = (long) points->x;
         *pi++ = (long) points->y;
         points++;
         if ( set != NULL ) *pi++ = *set++;
      }
   *pin = num;
   return;
}

/**************************************  Metafile Delimiter elements ***/

#ifdef PROTO
void m_begmf ( char *pc )
#else
void m_begmf ( pc )
char *pc;
#endif
{
   if ( cgmstate != MF_CLOSED )
   {
      CGMerror ("BEGMF", ERR_MFOPEN, FATAL, NULLSTR);
      return;
   }
   cgmstate = MF_DESC;
   CGMdefaults ( BEGMF, ZERO );

   CGMout ( BEGMF, pint, preal, pc );
   return;
}

#ifdef PROTO
void m_endmf ( void )
#else
void m_endmf ()
#endif
{
   cgmstate = MF_CLOSED;
   CGMout ( ENDMF, pint, preal, str );
   return;
}

#ifdef PROTO
void m_begpic ( char *pc )
#else
void m_begpic ( pc )
char *pc;
#endif
{
   if ( cgmstate != MF_DESC && cgmstate != PIC_CLOSED )
   {
      CGMerror ("BEGPIC", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   if ( cgmstate == MF_DESC )
      CGMdefaults ( MFDESC, ZERO );
   else
      CGMdefaults ( ENDPIC, ZERO );

   cgmstate = PIC_DESC;
   CGMout ( BEGPIC, pint, preal, pc );
   return;
}

#ifdef PROTO
void m_begpicbody ( void )
#else
void m_begpicbody ()
#endif
{
   if ( cgmstate != PIC_DESC )
   {
      CGMerror ("BEGPICBODY", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   cgmstate = PIC_OPEN;
   CGMout ( BEGPICBODY, pint, preal, str );
   return;
}

#ifdef PROTO
void m_endpic ( void )
#else
void m_endpic ()
#endif
{
   if ( cgmstate != PIC_DESC && cgmstate != PIC_OPEN )
   {
      CGMerror ("PIC_CLOSED", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   cgmstate = PIC_CLOSED;
   CGMout ( ENDPIC, pint, preal, str );
   return;
}

/************************************  Metafile Descriptor Elements ****/

#ifdef PROTO
void m_mfversion ( int verno )
#else
void m_mfversion ( verno )
int verno;
#endif
{
   if ( cgmstate != MF_DESC )
   {
      CGMerror ("MFVERSION", ERR_STATE, FATAL,NULLSTR);
      return;
   }
   if ( verno != 1 )
   {
      CGMerror ("MFVERSION", ERR_VERSION, FATAL, NULLSTR);
      return;
   }
   *pint = verno;
   CGMout ( MFVERSION, pint, preal, str );
   return;
}

#ifdef PROTO
void m_mfdesc ( char *pc )
#else
void m_mfdesc ( pc )
char *pc;
#endif
{
   if ( cgmstate != MF_DESC )
   {
      CGMerror ("MFDESC", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   CGMout ( MFDESC, pint, preal, pc );
   return;
}

#ifdef PROTO
void m_vdctype ( Enum type )
#else
void m_vdctype ( type )
Enum type;
#endif
{
   if ( cgmstate != MF_DESC )
   {
      CGMerror ("VDCTYPE", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   cur.vdc_type = type;
   CGMout ( VDCTYPE, pint, preal, str );
   return;
}

#ifdef PROTO
void m_integerprec ( int prec )
#else
void m_integerprec ( prec )
int prec;
#endif
{
   if ( cgmstate != MF_DESC )
   {
      CGMerror ("INTEGERPREC", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   cur.int_bits = prec;
   CGMout ( INTEGERPREC, pint, preal, str );
   return;
}

#ifdef PROTO
void m_realprec ( int max, int min, int defexp, Enum expald )
#else
void m_realprec ( max, min, defexp, expald )
int max, min, defexp;
Enum expald;
#endif
{
   if ( cgmstate != MF_DESC )
   {
      CGMerror ("REALPREC", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   cur.real_bits = max;
   cur.real_places = min;
   cur.real_defexp = defexp;
   cur.real_expald = expald;
   CGMout ( REALPREC, pint, preal, str );
   return;
}

#ifdef PROTO
void m_indexprec ( int prec )
#else
void m_indexprec ( prec )
int prec;
#endif
{
   if ( cgmstate != MF_DESC )
   {
      CGMerror ("INDEXPREC", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   cur.index_bits = prec;
   CGMout ( INDEXPREC, pint, preal, str );
   return;
}

#ifdef PROTO
void m_colrprec ( int prec )
#else
void m_colrprec ( prec )
int prec;
#endif
{
   if ( cgmstate != MF_DESC )
   {
      CGMerror ("COLRPREC", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   cur.col_bits = prec;
   CGMout ( COLRPREC, pint, preal, str );
   return;
}

#ifdef PROTO
void m_colrindexprec ( int prec )
#else
void m_colrindexprec ( prec )
int prec;
#endif
{
   if ( cgmstate != MF_DESC )
   {
      CGMerror ("COLRINDECPREC", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   cur.colind_bits = prec;
   CGMout ( COLRINDEXPREC, pint, preal, str );
   return;
}

#ifdef PROTO
void m_maxcolrindex ( int index )
#else
void m_maxcolrindex ( index )
int index;
#endif
{
   if ( cgmstate != MF_DESC )
   {
      CGMerror ("MAXCOLRINDEX", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   cur.max_colind = index;
   CGMout ( MAXCOLRINDEX, pint, preal, str );
   return;
}

#ifdef PROTO
void m_colrvalueext ( RGBcolour min, RGBcolour max )
#else
void m_colrvalueext ( min, max )
RGBcolour min, max;
#endif
{
   if ( cgmstate != MF_DESC )
   {
      CGMerror ("COLRVALUEEXT", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   cur.min_rgb.red = min.red;
   cur.min_rgb.green = min.green;
   cur.min_rgb.blue = min.blue;
   cur.max_rgb.red = max.red;
   cur.max_rgb.green = max.green;
   cur.max_rgb.blue = max.blue;
   CGMout ( COLRVALUEEXT, pint, preal, str );
   return;
}

#ifdef PROTO
void m_mfelemlist ( int num, Code *elemlist )
#else
void m_mfelemlist ( num, elemlist )
int num;
Code *elemlist;
#endif
{
   long *pi=pint+1;
   register int i;

   if ( cgmstate != MF_DESC )
   {
      CGMerror ("MFELEMLIST", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   for (i=0; i<num; i++)
   {
      *pi++ = *elemlist++;
   }
   *pint = num;
   CGMout ( MFELEMLIST, pint, preal, str );
   return;
}

#ifdef PROTO
void m_begmfdefaults ( void )
#else
void m_begmfdefaults ()
#endif
{
   if ( cgmstate != MF_DESC )
   {
      CGMerror ("BEGMFDEFAULTS", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   cgmstate = MF_DEFAULTS;
   CGMout ( BEGMFDEFAULTS, pint, preal, str );
   return;
}

#ifdef PROTO
void m_endmfdefaults ( void )
#else
void m_endmfdefaults ()
#endif
{
   if ( cgmstate != MF_DEFAULTS )
   {
      CGMerror ("ENDMFDEFAULTS", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   cgmstate = MF_DESC;
   CGMout ( ENDMFDEFAULTS, pint, preal, str );
   return;
}

#ifdef PROTO
void m_fontlist ( int num, char **fontname )
#else
void m_fontlist ( num, fontname )
int num;
char **fontname ;
#endif
{
   long *pi = pint+1;
   int i, j = 0;

   if ( cgmstate != MF_DESC )
   {
      CGMerror ("FONTLIST", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   for ( i=0; i<num; i++ )
   {
      strcpy( &str[j], *fontname );
      j += strlen( *fontname ) + 1;
      *pi++ = j;
      fontname++;
   }
   *pint = j;
#ifdef FONTSYSTEM
   FNTflist(pi, pint, str, &cgmfonts);
#endif
   CGMout ( FONTLIST, pint, preal, str );
   return;
}

#ifdef PROTO
void m_charsetlist ( int num, int *charset, char **set )
#else
void m_charsetlist ( num, charset, set )
int num, *charset;
char **set;
#endif
{
   long *pi = pint+1;
   int i, j = 0;

   if ( cgmstate != MF_DESC )
   {
      CGMerror ("CHARSETLIST", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   for ( i=0; i<num; i++ )
   {
      *pi = *charset++;
      strcpy( &str[j], *set );
      j += strlen( *set ) + 1;
      *pi++ = j;
      set++;
   }
   *pint = j;
#ifdef FONTSYSTEM
   FNTclist(pi, pint, str, &cgmcsets);
#endif
   CGMout ( CHARSETLIST, pint, preal, str );
   return;
}

#ifdef PROTO
void m_charcoding ( Enum coding )
#else
void m_charcoding ( coding )
Enum coding;
#endif
{
   if ( cgmstate != MF_DESC )
   {
      CGMerror ("CHARCODING", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   *pint = coding;
   CGMout ( CHARCODING, pint, preal, str );
   return;
}

/*************************************  Picture Descriptor elements ****/

#ifdef PROTO
void m_scalemode ( Enum mode, float factor )
#else
void m_scalemode ( mode, factor )
Enum mode;
float factor;
#endif
{
   if ( cgmstate != PIC_DESC && cgmstate != MF_DEFAULTS )
   {
      CGMerror ("SCALEMODE", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   cur.scale_mode = mode;
   cur.scale_factor = factor;
   CGMout ( SCALEMODE, pint, preal, str );
   return;
}

#ifdef PROTO
void m_colrmode ( Enum mode )
#else
void m_colrmode ( mode )
Enum mode;
#endif
{
   if ( cgmstate != PIC_DESC && cgmstate != MF_DEFAULTS )
   {
      CGMerror ("COLRMODE", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   cur.color_mode = mode;
   CGMout ( COLRMODE, pint, preal, str );
   return;
}

#ifdef PROTO
void m_linewidthmode ( Enum mode )
#else
void m_linewidthmode ( mode )
Enum mode;
#endif
{
   if ( cgmstate != PIC_DESC && cgmstate != MF_DEFAULTS )
   {
      CGMerror ("LINEWIDTHMODE", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   cur.linewidth_mode = mode;
   CGMout ( LINEWIDTHMODE, pint, preal, str );
   return;
}

#ifdef PROTO
void m_markersizemode ( Enum mode )
#else
void m_markersizemode ( mode )
Enum mode;
#endif
{
   if ( cgmstate != PIC_DESC && cgmstate != MF_DEFAULTS )
   {
      CGMerror ("MARKERSIZEMODE", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   cur.markersize_mode = mode;
   CGMout ( MARKERSIZEMODE, pint, preal, str );
   return;
}

#ifdef PROTO
void m_edgewidthmode ( Enum mode )
#else
void m_edgewidthmode ( mode )
Enum mode;
#endif
{
   if ( cgmstate != PIC_DESC && cgmstate != MF_DEFAULTS )
   {
      CGMerror ("EDGEWIDTHMODE", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   cur.edgewidth_mode = mode;
   CGMout ( EDGEWIDTHMODE, pint, preal, str );
   return;
}

#ifdef PROTO
void m_vdcext ( Point a, Point b )
#else
void m_vdcext ( a, b )
Point a, b;
#endif
{
   if ( cgmstate != PIC_DESC && cgmstate != MF_DEFAULTS )
   {
      CGMerror ("VDCEXTENT", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   if ( cur.vdc_type == REAL )
   {
      cur.vdc_extent.a.x.real = a.x;
      cur.vdc_extent.a.y.real = a.y;
      cur.vdc_extent.b.x.real = b.x;
      cur.vdc_extent.b.y.real = b.y;
   }
   else
   {
      cur.vdc_extent.a.x.intr = (long) a.x;
      cur.vdc_extent.a.y.intr = (long) a.y;
      cur.vdc_extent.b.x.intr = (long) b.x;
      cur.vdc_extent.b.y.intr = (long) b.y;
   }
   CGMout ( VDCEXT, pint, preal, str );
   return;
}

#ifdef PROTO
void m_backcolr ( RGBcolour backcol )
#else
void m_backcolr ( backcol )
RGBcolour backcol;
#endif
{
   if ( cgmstate != PIC_DESC && cgmstate != MF_DEFAULTS )
   {
      CGMerror ("BACKCOLR", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   cur.back.red = backcol.red;
   cur.back.green = backcol.green;
   cur.back.blue = backcol.blue;
   CGMout ( BACKCOLR, pint, preal, str );
   return;
}

/*************************************************  Control elements ***/

#ifdef PROTO
void m_vdcintegerprec ( int prec )
#else
void m_vdcintegerprec ( prec )
int prec;
#endif
{
   if ( cgmstate != PIC_OPEN && cgmstate != MF_DEFAULTS )
   {
      CGMerror ("VDCINTEGERPREC", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   cur.vdcint_bits = prec;
   CGMout ( VDCINTEGERPREC, pint, preal, str );
   return;
}

#ifdef PROTO
void m_vdcrealprec ( int max, int min, int defexp, Enum expald )
#else
void m_vdcrealprec ( max, min, defexp, expald )
int max, min, defexp;
Enum expald;
#endif
{
   if ( cgmstate != PIC_OPEN && cgmstate != MF_DEFAULTS )
   {
      CGMerror ("VDCREALPREC", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   cur.vdc_bits = max;
   cur.vdc_places = min;
   cur.vdc_defexp = defexp;
   cur.vdc_expald = expald;
   CGMout ( VDCREALPREC, pint, preal, str );
   return;
}

#ifdef PROTO
void m_auxcolr ( Colour col )
#else
void m_auxcolr ( col )
Colour col;
#endif
{
   if ( cgmstate != PIC_OPEN && cgmstate != MF_DEFAULTS &&
        cgmstate != TEXT_OPEN )
   {
      CGMerror ("AUXCOLR", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   cur.aux.red = col.red;
   cur.aux.green = col.green;
   cur.aux.blue = col.blue;
   CGMout ( AUXCOLR, pint, preal, str );
   return;
}

#ifdef PROTO
void m_transparency ( Enum trans )
#else
void m_transparency ( trans )
Enum trans;
#endif
{
   if ( cgmstate != PIC_OPEN && cgmstate != MF_DEFAULTS &&
        cgmstate != TEXT_OPEN )
   {
      CGMerror ("TRANSPARENCY", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   cur.transparency = trans;
   CGMout ( TRANSPARENCY, pint, preal, str );
   return;
}

#ifdef PROTO
void m_cliprect ( Point a, Point b )
#else
void m_cliprect ( a, b )
Point a, b;
#endif
{
   if ( cgmstate != PIC_OPEN && cgmstate != MF_DEFAULTS )
   {
      CGMerror ("CLIPRECT", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   if ( cur.vdc_type == REAL )
   {
      cur.clip_rect.a.x.real = a.x;
      cur.clip_rect.a.y.real = a.y;
      cur.clip_rect.b.x.real = b.x;
      cur.clip_rect.b.y.real = b.y;
   }
   else
   {
      cur.clip_rect.a.x.intr = (long) a.x;
      cur.clip_rect.a.y.intr = (long) a.y;
      cur.clip_rect.b.x.intr = (long) b.x;
      cur.clip_rect.b.y.intr = (long) b.y;
   }
   CGMout ( CLIPRECT, pint, preal, str );
   return;
}

#ifdef PROTO
void m_clip ( Enum clip )
#else
void m_clip ( clip )
Enum clip;
#endif
{
   if ( cgmstate != PIC_OPEN && cgmstate != MF_DEFAULTS )
   {
      CGMerror ("CLIP", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   cur.clip_ind = clip;
   CGMout ( CLIP, pint, preal, str );
   return;
}

/********************************************  Graphical Primitives ****/

#ifdef PROTO
void m_line ( int num, Point *points )
#else
void m_line ( num, points )
int num;
Point *points;
#endif
{
   if ( cgmstate != PIC_OPEN )
   {
      CGMerror ("LINE", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   CGMconvert_points ( num, points, pint, preal, (Enum *) NULL);
   CGMout ( LINE, pint, preal, str );
   return;
}

#ifdef PROTO
void m_disjtline ( int num, Point *points )
#else
void m_disjtline ( num, points )
int num;
Point *points;
#endif
{
   if ( cgmstate != PIC_OPEN )
   {
      CGMerror ("DISJTLINE", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   CGMconvert_points ( num, points, pint, preal, (Enum *) NULL);
   CGMout ( DISJTLINE, pint, preal, str );
   return;
}

#ifdef PROTO
void m_marker ( int num, Point *points )
#else
void m_marker ( num, points )
int num;
Point *points;
#endif
{
   if ( cgmstate != PIC_OPEN )
   {
      CGMerror ("MARKER", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   CGMconvert_points ( num, points, pint, preal, (Enum *) NULL);
   CGMout ( MARKER, pint, preal, str );
   return;
}

#ifdef PROTO
void m_text ( Point a, Enum final, char *text )
#else
void m_text ( a, final, text )
Point a;
Enum final;
char *text;
#endif
{
   long *pi = pint+1;
   float *pr = preal;

   if ( cgmstate != PIC_OPEN )
   {
      CGMerror ("TEXT", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   if ( cur.vdc_type == REAL )
   {
      *pr++ = a.x;
      *pr++ = a.y;
   }
   else
   {
      *pi++ = (long) a.x;
      *pi++ = (long) a.y;
   }
   *pint = final;
   CGMout ( TEXT, pint, preal, text );
   return;
}

#ifdef PROTO
void m_restrtext ( Point a, Point b, Enum final, char *text )
#else
void m_restrtext ( a, b, final, text )
Point a, b;
Enum final;
char *text;
#endif
{
   long *pi = pint+1;
   float *pr = preal;

   if ( cgmstate != PIC_OPEN )
   {
      CGMerror ("RESTRTEXT", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   if ( cur.vdc_type == REAL )
   {
      *pr++ = a.x;
      *pr++ = a.y;
      *pr++ = b.x;
      *pr++ = b.y;
   }
   else
   {
      *pi++ = (long) a.x;
      *pi++ = (long) a.y;
      *pi++ = (long) b.x;
      *pi++ = (long) b.y;
   }
   *pint = final;
   CGMout ( RESTRTEXT, pint, preal, text );
   return;
}

#ifdef PROTO
void m_apndtext ( Enum final, char *text )
#else
void m_apndtext ( final, text )
Enum final;
char *text;
#endif
{
   if ( cgmstate != TEXT_OPEN )
   {
      CGMerror ("APNDTEXT", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   *pint = final;
   CGMout ( APNDTEXT, pint, preal, text );
   return;
}

#ifdef PROTO
void m_polygon ( int num, Point *points )
#else
void m_polygon ( num, points )
int num;
Point *points;
#endif
{
   if ( cgmstate != PIC_OPEN )
   {
      CGMerror ("POLYGON", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   CGMconvert_points ( num, points, pint, preal, (Enum *) NULL);
   CGMout ( POLYGON, pint, preal, str );
   return;
}

#ifdef PROTO
void m_polygonset ( int num, Point *points, Enum *set )
#else
void m_polygonset ( num, points, set )
int num;
Point *points;
Enum *set;
#endif
{
   if ( cgmstate != PIC_OPEN )
   {
      CGMerror ("POLYGONSET", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   CGMconvert_points ( num, points, pint, preal, set);
   CGMout ( POLYGONSET, pint, preal, str );
   return;
}

#ifdef PROTO
void m_cellarray ( Point p, Point q, Point r, int nx, int ny,
                   Indexcolour *ilist, RGBcolour *clist )
#else
void m_cellarray ( p, q, r, nx, ny, ilist, clist )
Point p, q, r;
int nx, ny;
Indexcolour *ilist;
RGBcolour *clist;
#endif
{
   long *pi = pint+1;
   float *pr = preal;
   int i;

   if ( cgmstate != PIC_OPEN )
   {
      CGMerror ("CELLARRAY", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   if ( cur.vdc_type == REAL )
   {
      *pr++ = p.x;
      *pr++ = p.y;
      *pr++ = q.x;
      *pr++ = q.y;
      *pr++ = r.x;
      *pr++ = r.y;
      pi += 6;
   }
   else
   {
      *pi++ = (long) p.x;
      *pi++ = (long) p.y;
      *pi++ = (long) q.x;
      *pi++ = (long) q.y;
      *pi++ = (long) r.x;
      *pi++ = (long) r.y;
   }
   *pi++ = nx;
   *pi++ = ny;
   *pi++ = 0;
   *pint = nx*ny;
   for (i=0; i<(nx*ny); i++)
   {
         if ( pi+(cur.color_mode == DIRECT ? 3 : 1) > pimax )
#ifdef FIXED_BUFFER
         {
           *pint = - i;
           CGMout ( CELLARRAY, pint, preal, str );
           pi = pint+1;
         }
#else
           pi = CGMialloc(pi, nx*ny*(cur.color_mode == DIRECT ? 3 : 1) );
#endif
      if ( cur.color_mode == DIRECT )
      {
         *pi++ = clist->red;
         *pi++ = clist->green;
         *pi++ = clist->blue;
         clist++;
      }
      else *pi++ = *ilist++;
   }
   CGMout ( CELLARRAY, pint, preal, str );
   return;
}

#ifdef PROTO
void m_gdp ( int gdpid, int num, Point *points, char *data )
#else
void m_gdp ( gdpid, num, points, data )
int gdpid, num;
Point *points;
char *data;
#endif
{
   if ( cgmstate != PIC_OPEN )
   {
      CGMerror ("GDP", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   *pint = num;
   CGMconvert_points ( num, points, pint+1, preal, (Enum *) NULL);
   *(pint+1) = gdpid;
   CGMout ( GDP, pint, preal, data );
   return;
}

#ifdef PROTO
void m_rect ( Point a, Point b )
#else
void m_rect ( a, b )
Point a, b;
#endif
{
   long *pi = pint+1;
   float *pr = preal;

   if ( cgmstate != PIC_OPEN )
   {
      CGMerror ("RECT", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   if ( cur.vdc_type == REAL )
   {
      *pr++ = a.x;
      *pr++ = a.y;
      *pr++ = b.x;
      *pr++ = b.y;
   }
   else
   {
      *pi++ = (long) a.x;
      *pi++ = (long) a.y;
      *pi++ = (long) b.x;
      *pi++ = (long) b.y;
   }
   CGMout ( RECT, pint, preal, str );
   return;
}

#ifdef PROTO
void m_circle ( Point a, Point b, float rad )
#else
void m_circle ( a, b, rad )
Point a, b;
float rad;
#endif
{
   long *pi = pint+1;
   float *pr = preal;

   if ( cgmstate != PIC_OPEN )
   {
      CGMerror ("CIRCLE", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   if ( cur.vdc_type == REAL )
   {
      *pr++ = a.x;
      *pr++ = a.y;
      *pr++ = b.x;
      *pr++ = b.y;
      *pr++ = rad;
   }
   else
   {
      *pi++ = (long) a.x;
      *pi++ = (long) a.y;
      *pi++ = (long) b.x;
      *pi++ = (long) b.y;
      *pi++ = (long) rad;
   }
   CGMout ( CIRCLE, pint, preal, str );
   return;
}

#ifdef PROTO
void m_arc3pt ( Point a, Point b, Point c )
#else
void m_arc3pt ( a, b, c )
Point a, b, c;
#endif
{
   long *pi = pint+1;
   float *pr = preal;

   if ( cgmstate != PIC_OPEN )
   {
      CGMerror ("ARC3PT", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   if ( cur.vdc_type == REAL )
   {
      *pr++ = a.x;
      *pr++ = a.y;
      *pr++ = b.x;
      *pr++ = b.y;
      *pr++ = c.x;
      *pr++ = c.y;
   }
   else
   {
      *pi++ = (long) a.x;
      *pi++ = (long) a.y;
      *pi++ = (long) b.x;
      *pi++ = (long) b.y;
      *pi++ = (long) c.x;
      *pi++ = (long) c.y;
   }
   CGMout ( ARC3PT, pint, preal, str );
   return;
}

#ifdef PROTO
void m_arc3ptclose ( Point a, Point b, Point c, Enum close )
#else
void m_arc3ptclose ( a, b, c, close )
Point a, b, c;
Enum close;
#endif
{
   long *pi = pint+1;
   float *pr = preal;

   if ( cgmstate != PIC_OPEN )
   {
      CGMerror ("ARC3PTCLOSE", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   if ( cur.vdc_type == REAL )
   {
      *pr++ = a.x;
      *pr++ = a.y;
      *pr++ = b.x;
      *pr++ = b.y;
      *pr++ = c.x;
      *pr++ = c.y;
   }
   else
   {
      *pi++ = (long) a.x;
      *pi++ = (long) a.y;
      *pi++ = (long) b.x;
      *pi++ = (long) b.y;
      *pi++ = (long) c.x;
      *pi++ = (long) c.y;
   }
   *(pi+6) = close;
   CGMout ( ARC3PTCLOSE, pint, preal, str );
   return;
}

#ifdef PROTO
void m_arcctr ( Point centre, Point start, Point end, float rad )
#else
void m_arcctr ( centre, start, end, rad )
Point centre, start, end;
float rad;
#endif
{
   long *pi = pint+1;
   float *pr = preal;

   if ( cgmstate != PIC_OPEN )
   {
      CGMerror ("ARCCTR", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   if ( cur.vdc_type == REAL )
   {
      *pr++ = centre.x;
      *pr++ = centre.y;
      *pr++ = start.x;
      *pr++ = start.y;
      *pr++ = end.x;
      *pr++ = end.y;
      *pr++ = rad;
   }
   else
   {
      *pi++ = (long) centre.x;
      *pi++ = (long) centre.y;
      *pi++ = (long) start.x;
      *pi++ = (long) start.y;
      *pi++ = (long) end.x;
      *pi++ = (long) end.y;
      *pi++ = (long) rad;
   }
   CGMout ( ARCCTR, pint, preal, str );
   return;
}

#ifdef PROTO
void m_arcctrclose ( Point centre, Point start, Point end,
                     float rad, Enum close )
#else
void m_arcctrclose ( centre, start, end, rad, close )
Point centre, start, end;
float rad;
Enum close;
#endif
{
   long *pi = pint+1;
   float *pr = preal;

   if ( cgmstate != PIC_OPEN )
   {
      CGMerror ("ARCCTRCLOSE", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   if ( cur.vdc_type == REAL )
   {
      *pr++ = centre.x;
      *pr++ = centre.y;
      *pr++ = start.x;
      *pr++ = start.y;
      *pr++ = end.x;
      *pr++ = end.y;
      *pr++ = rad;
   }
   else
   {
      *pi++ = (long) centre.x;
      *pi++ = (long) centre.y;
      *pi++ = (long) start.x;
      *pi++ = (long) start.y;
      *pi++ = (long) end.x;
      *pi++ = (long) end.y;
      *pi++ = (long) rad;
   }
   *(pi+7) = close;
   CGMout ( ARCCTRCLOSE, pint, preal, str );
   return;
}

#ifdef PROTO
void m_ellipse ( Point centre, Point cjd1, Point cjd2 )
#else
void m_ellipse ( centre, cjd1, cjd2 )
Point centre, cjd1, cjd2;
#endif
{
   long *pi = pint+1;
   float *pr = preal;

   if ( cgmstate != PIC_OPEN )
   {
      CGMerror ("ELLIPSE", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   if ( cur.vdc_type == REAL )
   {
      *pr++ = centre.x;
      *pr++ = centre.y;
      *pr++ = cjd1.x;
      *pr++ = cjd1.y;
      *pr++ = cjd2.x;
      *pr++ = cjd2.y;
   }
   else
   {
      *pi++ = (long) centre.x;
      *pi++ = (long) centre.y;
      *pi++ = (long) cjd1.x;
      *pi++ = (long) cjd1.y;
      *pi++ = (long) cjd2.x;
      *pi++ = (long) cjd2.y;
   }
   CGMout ( ELLIPSE, pint, preal, str );
   return;
}

#ifdef PROTO
void m_elliparc ( Point centre, Point cjd1, Point cjd2,
                  Point start, Point end )
#else
void m_elliparc ( centre, cjd1, cjd2, start, end )
Point centre, cjd1, cjd2, start, end;
#endif
{
   long *pi = pint+1;
   float *pr = preal;

   if ( cgmstate != PIC_OPEN )
   {
      CGMerror ("ELLIPARC", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   if ( cur.vdc_type == REAL )
   {
      *pr++ = centre.x;
      *pr++ = centre.y;
      *pr++ = cjd1.x;
      *pr++ = cjd1.y;
      *pr++ = cjd2.x;
      *pr++ = cjd2.y;
      *pr++ = start.x;
      *pr++ = start.y;
      *pr++ = end.x;
      *pr++ = end.y;
   }
   else
   {
      *pi++ = (long) centre.x;
      *pi++ = (long) centre.y;
      *pi++ = (long) cjd1.x;
      *pi++ = (long) cjd1.y;
      *pi++ = (long) cjd2.x;
      *pi++ = (long) cjd2.y;
      *pi++ = (long) start.x;
      *pi++ = (long) start.y;
      *pi++ = (long) end.x;
      *pi++ = (long) end.y;
   }
   CGMout ( ELLIPARC, pint, preal, str );
   return;
}

#ifdef PROTO
void m_elliparcclose ( Point centre, Point cjd1, Point cjd2,
                       Point start, Point end, Enum close )
#else
void m_elliparcclose ( centre, cjd1, cjd2, start, end, close )
Point centre, cjd1, cjd2, start, end;
Enum close;
#endif
{
   long *pi = pint+1;
   float *pr = preal;

   if ( cgmstate != PIC_OPEN )
   {
      CGMerror ("ELLIPARCCLOSE", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   if ( cur.vdc_type == REAL )
   {
      *pr++ = centre.x;
      *pr++ = centre.y;
      *pr++ = cjd1.x;
      *pr++ = cjd1.y;
      *pr++ = cjd2.x;
      *pr++ = cjd2.y;
      *pr++ = start.x;
      *pr++ = start.y;
      *pr++ = end.x;
      *pr++ = end.y;
   }
   else
   {
      *pi++ = (long) centre.x;
      *pi++ = (long) centre.y;
      *pi++ = (long) cjd1.x;
      *pi++ = (long) cjd1.y;
      *pi++ = (long) cjd2.x;
      *pi++ = (long) cjd2.y;
      *pi++ = (long) start.x;
      *pi++ = (long) start.y;
      *pi++ = (long) end.x;
      *pi++ = (long) end.y;
   }
   *(pi+10) = close;
   CGMout ( ELLIPARCCLOSE, pint, preal, str );
   return;
}

/******************************************************  Attributes ****/

#ifdef PROTO
void m_lineindex ( Index index )
#else
void m_lineindex ( index )
Index index;
#endif
{
   if ( cgmstate != PIC_OPEN && cgmstate != MF_DEFAULTS )
   {
      CGMerror ("LINEINDEX", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   curatt.line_ind = index;
   CGMout ( LINEINDEX, pint, preal, str );
   return;
}

#ifdef PROTO
void m_linetype ( Index type )
#else
void m_linetype ( type )
Index type;
#endif
{
   if ( cgmstate != PIC_OPEN && cgmstate != MF_DEFAULTS )
   {
      CGMerror ("LINETYPE", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   curatt.line_type = type;
   CGMout ( LINETYPE, pint, preal, str );
   return;
}

#ifdef PROTO
void m_linewidth ( float width )
#else
void m_linewidth ( width )
float width;
#endif
{
   if ( cgmstate != PIC_OPEN && cgmstate != MF_DEFAULTS )
   {
      CGMerror ("LINEWIDTH", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   if ( cur.vdc_type == REAL )
      curatt.line_width.real = width;
   else
      curatt.line_width.intr = width;
   CGMout ( LINEWIDTH, pint, preal, str );
   return;
}

#ifdef PROTO
void m_linecolr ( Colour col )
#else
void m_linecolr ( col )
Colour col;
#endif
{
   if ( cgmstate != PIC_OPEN && cgmstate != MF_DEFAULTS )
   {
      CGMerror ("LINECOLR", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   curatt.line = col;
   CGMout ( LINECOLR, pint, preal, str );
   return;
}

#ifdef PROTO
void m_markerindex ( Index index )
#else
void m_markerindex ( index )
Index index;
#endif
{
   if ( cgmstate != PIC_OPEN && cgmstate != MF_DEFAULTS )
   {
      CGMerror ("MARKERINDEX", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   curatt.mark_ind = index;
   CGMout ( MARKERINDEX, pint, preal, str );
   return;
}

#ifdef PROTO
void m_markertype ( Index type )
#else
void m_markertype ( type )
Index type;
#endif
{
   if ( cgmstate != PIC_OPEN && cgmstate != MF_DEFAULTS )
   {
      CGMerror ("MARKERTYPE", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   curatt.mark_type = type;
   CGMout ( MARKERTYPE, pint, preal, str );
   return;
}

#ifdef PROTO
void m_markersize ( float size )
#else
void m_markersize ( size )
float size;
#endif
{
   if ( cgmstate != PIC_OPEN && cgmstate != MF_DEFAULTS )
   {
      CGMerror ("MARKERSIZE", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   if ( cur.vdc_type == REAL )
      curatt.mark_size.real = size;
   else
      curatt.mark_size.intr = (long) size;
   CGMout ( MARKERSIZE, pint, preal, str );
   return;
}

#ifdef PROTO
void m_markercolr ( Colour col )
#else
void m_markercolr ( col )
Colour col;
#endif
{
   if ( cgmstate != PIC_OPEN && cgmstate != MF_DEFAULTS )
   {
      CGMerror ("MARKERCOLR", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   curatt.marker = col;
   CGMout ( MARKERCOLR, pint, preal, str );
   return;
}

#ifdef PROTO
void m_textindex ( Index index )
#else
void m_textindex ( index )
Index index;
#endif
{
   if ( cgmstate != PIC_OPEN && cgmstate != MF_DEFAULTS &&
        cgmstate != TEXT_OPEN )
   {
      CGMerror ("TEXTINDEX", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   curatt.text_ind = index;
   CGMout ( TEXTINDEX, pint, preal, str );
   return;
}

#ifdef PROTO
void m_textfontindex ( Index index )
#else
void m_textfontindex ( index )
Index index;
#endif
{
   if ( cgmstate != PIC_OPEN && cgmstate != MF_DEFAULTS &&
        cgmstate != TEXT_OPEN )
   {
      CGMerror ("TEXTFONTINDEX", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   curatt.text_font = index;
   CGMout ( TEXTFONTINDEX, pint, preal, str );
   return;
}

#ifdef PROTO
void m_textprec ( Enum prec )
#else
void m_textprec ( prec )
Enum prec;
#endif
{
   if ( cgmstate != PIC_OPEN && cgmstate != MF_DEFAULTS &&
        cgmstate != TEXT_OPEN )
   {
      CGMerror ("TEXTPREC", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   curatt.text_prec = prec;
   CGMout ( TEXTPREC, pint, preal, str );
   return;
}

#ifdef PROTO
void m_charexpan ( float factor )
#else
void m_charexpan ( factor )
float factor;
#endif
{
   if ( cgmstate != PIC_OPEN && cgmstate != MF_DEFAULTS &&
        cgmstate != TEXT_OPEN )
   {
      CGMerror ("CHAREXPAN", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   curatt.char_exp = factor;
   CGMout ( CHAREXPAN, pint, preal, str );
   return;
}

#ifdef PROTO
void m_charspace ( float spacing )
#else
void m_charspace ( spacing )
float spacing;
#endif
{
   if ( cgmstate != PIC_OPEN && cgmstate != MF_DEFAULTS &&
        cgmstate != TEXT_OPEN )
   {
      CGMerror ("CHARSPACE", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   curatt.char_space = spacing;
   CGMout ( CHARSPACE, pint, preal, str );
   return;
}

#ifdef PROTO
void m_textcolr ( Colour col )
#else
void m_textcolr ( col )
Colour col;
#endif
{
   if ( cgmstate != PIC_OPEN && cgmstate != MF_DEFAULTS &&
        cgmstate != TEXT_OPEN )
   {
      CGMerror ("TEXTCOLR", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   curatt.text = col;
   CGMout ( TEXTCOLR, pint, preal, str );
   return;
}

#ifdef PROTO
void m_charheight ( float height )
#else
void m_charheight ( height )
float height;
#endif
{
   if ( cgmstate != PIC_OPEN && cgmstate != MF_DEFAULTS &&
        cgmstate != TEXT_OPEN )
   {
      CGMerror ("CHARHEIGHT", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   if ( cur.vdc_type == REAL )
      curatt.char_height.real = height;
   else
      curatt.char_height.intr = (long) height;
   CGMout ( CHARHEIGHT, pint, preal, str );
   return;
}

#ifdef PROTO
void m_charori ( Vector upvec, Vector basevec )
#else
void m_charori ( upvec, basevec )
Vector upvec, basevec;
#endif
{
   if ( cgmstate != PIC_OPEN && cgmstate != MF_DEFAULTS )
   {
      CGMerror ("CHARORI", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   if ( cur.vdc_type == REAL )
   {
      curatt.char_up.x.real = upvec.x;
      curatt.char_up.y.real = upvec.y;
      curatt.char_base.x.real = basevec.x;
      curatt.char_base.y.real = basevec.y;
   }
   else
   {
      curatt.char_up.x.intr = (long) upvec.x;
      curatt.char_up.y.intr = (long) upvec.y;
      curatt.char_base.x.intr = (long) basevec.x;
      curatt.char_base.y.intr = (long) basevec.y;
   }
   CGMout ( CHARORI, pint, preal, str );
   return;
}

#ifdef PROTO
void m_textpath ( Enum path )
#else
void m_textpath ( path )
Enum path;
#endif
{
   if ( cgmstate != PIC_OPEN && cgmstate != MF_DEFAULTS )
   {
      CGMerror ("TEXTPATH", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   curatt.text_path = path;
   CGMout ( TEXTPATH, pint, preal, str );
   return;
}

#ifdef PROTO
void m_textalign ( Enum horiz, Enum vert, float conth, float contv )
#else
void m_textalign ( horiz, vert, conth, contv )
Enum horiz, vert;
float conth, contv;
#endif
{
   if ( cgmstate != PIC_OPEN && cgmstate != MF_DEFAULTS )
   {
      CGMerror ("TEXTALIGN", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   curatt.text_halign = horiz;
   curatt.text_valign = vert;
   curatt.text_hcont = conth;
   curatt.text_vcont = contv;
   CGMout ( TEXTALIGN, pint, preal, str );
   return;
}

#ifdef PROTO
void m_charsetindex ( Index index )
#else
void m_charsetindex ( index )
Index index;
#endif
{
   if ( cgmstate != PIC_OPEN && cgmstate != MF_DEFAULTS &&
        cgmstate != TEXT_OPEN )
   {
      CGMerror ("CHARSETINDEX", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   curatt.char_set = index;
   CGMout ( CHARSETINDEX, pint, preal, str );
   return;
}

#ifdef PROTO
void m_altcharsetindex ( Index index )
#else
void m_altcharsetindex ( index )
Index index;
#endif
{
   if ( cgmstate != PIC_OPEN && cgmstate != MF_DEFAULTS &&
        cgmstate != TEXT_OPEN )
   {
      CGMerror ("ALTCHARSETINDEX", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   curatt.altchar_set = index;
   CGMout ( ALTCHARSETINDEX, pint, preal, str );
   return;
}

#ifdef PROTO
void m_fillindex ( Index index )
#else
void m_fillindex ( index )
Index index;
#endif
{
   if ( cgmstate != PIC_OPEN && cgmstate != MF_DEFAULTS )
   {
      CGMerror ("FILLINDEX", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   curatt.fill_ind = index;
   CGMout ( FILLINDEX, pint, preal, str );
   return;
}

#ifdef PROTO
void m_intstyle ( Enum style )
#else
void m_intstyle ( style )
Enum style;
#endif
{
   if ( cgmstate != PIC_OPEN && cgmstate != MF_DEFAULTS )
   {
      CGMerror ("INTSTYLE", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   curatt.int_style = style;
   CGMout ( INTSTYLE, pint, preal, str );
   return;
}

#ifdef PROTO
void m_fillcolr ( Colour col )
#else
void m_fillcolr ( col )
Colour col;
#endif
{
   if ( cgmstate != PIC_OPEN && cgmstate != MF_DEFAULTS )
   {
      CGMerror ("FILLCOLR", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   curatt.fill = col;
   CGMout ( FILLCOLR, pint, preal, str );
   return;
}

#ifdef PROTO
void m_hatchindex ( Index index )
#else
void m_hatchindex ( index )
Index index;
#endif
{
   if ( cgmstate != PIC_OPEN && cgmstate != MF_DEFAULTS )
   {
      CGMerror ("HATCHINDEX", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   curatt.hatch_ind = index;
   CGMout ( HATCHINDEX, pint, preal, str );
   return;
}

#ifdef PROTO
void m_patindex ( Index index )
#else
void m_patindex ( index )
Index index;
#endif
{
   if ( cgmstate != PIC_OPEN && cgmstate != MF_DEFAULTS )
   {
      CGMerror ("PATINDEX", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   curatt.pat_ind = index;
   CGMout ( PATINDEX, pint, preal, str );
   return;
}

#ifdef PROTO
void m_edgeindex ( Index index )
#else
void m_edgeindex ( index )
Index index;
#endif
{
   if ( cgmstate != PIC_OPEN && cgmstate != MF_DEFAULTS )
   {
      CGMerror ("EDGEINDEX", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   curatt.edge_ind = index;
   CGMout ( EDGEINDEX, pint, preal, str );
   return;
}

#ifdef PROTO
void m_edgetype ( Index type )
#else
void m_edgetype ( type )
Index type;
#endif
{
   if ( cgmstate != PIC_OPEN && cgmstate != MF_DEFAULTS )
   {
      CGMerror ("EDGETYPE", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   curatt.edge_type = type;
   CGMout ( EDGETYPE, pint, preal, str );
   return;
}

#ifdef PROTO
void m_edgewidth ( float width )
#else
void m_edgewidth ( width )
float width;
#endif
{
   if ( cgmstate != PIC_OPEN && cgmstate != MF_DEFAULTS )
   {
      CGMerror ("EDGEWIDTH", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   if ( cur.vdc_type == REAL )
      curatt.edge_width.real = width;
   else
      curatt.edge_width.intr = (long) width;
   CGMout ( EDGEWIDTH, pint, preal, str );
   return;
}

#ifdef PROTO
void m_edgecolr ( Colour col )
#else
void m_edgecolr ( col )
Colour col;
#endif
{
   if ( cgmstate != PIC_OPEN && cgmstate != MF_DEFAULTS )
   {
      CGMerror ("DEGECOLR", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   curatt.edge = col;
   CGMout ( EDGECOLR, pint, preal, str );
   return;
}

#ifdef PROTO
void m_edgevis ( Enum vis )
#else
void m_edgevis ( vis )
Enum vis;
#endif
{
   if ( cgmstate != PIC_OPEN && cgmstate != MF_DEFAULTS )
   {
      CGMerror ("EDGEVIS", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   curatt.edge_vis = vis;
   CGMout ( EDGEVIS, pint, preal, str );
   return;
}

#ifdef PROTO
void m_fillrefpt ( Point refpoint )
#else
void m_fillrefpt ( refpoint )
Point refpoint;
#endif
{
   if ( cgmstate != PIC_OPEN && cgmstate != MF_DEFAULTS )
   {
      CGMerror ("FILLREFPT", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   if ( cur.vdc_type == REAL )
   {
      curatt.fill_ref.x.real = refpoint.x;
      curatt.fill_ref.y.real = refpoint.y;
   }
   else
   {
      curatt.fill_ref.x.intr = (long) refpoint.x;
      curatt.fill_ref.y.intr = (long) refpoint.y;
   }
   CGMout ( FILLREFPT, pint, preal, str );
   return;
}

#ifdef PROTO
void m_pattable ( Index index, int nx, int ny, Prec locprec, Colour *clist )
#else
void m_pattable ( index, nx, ny, locprec, clist )
Index index;
int nx, ny;
Prec locprec;
Colour *clist;
#endif
{
   long *pi = pint+1;
   register int i;

   if ( cgmstate != PIC_OPEN && cgmstate != MF_DEFAULTS )
   {
      CGMerror ("PATTABLE", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   *pi++ = index;
   *pi++ = nx;
   *pi++ = ny;
   *pi++ = locprec;
   *pint = nx*ny;
   if ( cur.color_mode == DIRECT )
      for ( i=0; i<*pint; i++ )
      {
         *pi++ = clist->red;
         *pi++ = clist->green;
         *pi++ = clist->blue;
         clist++;
      }
   else
      for ( i=0; i<*pint; i++, clist++ ) *pi++ = clist->index;

   CGMout ( PATTABLE, pint, preal, str );
   return;
}

#ifdef PROTO
void m_patsize ( Vector height, Vector width )
#else
void m_patsize ( height, width )
Vector height, width;
#endif
{
   if ( cgmstate != PIC_OPEN && cgmstate != MF_DEFAULTS )
   {
      CGMerror ("PATSIZE", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   if ( cur.vdc_type == REAL )
   {
      curatt.pat_size.a.x.real = height.x;
      curatt.pat_size.a.y.real = height.y;
      curatt.pat_size.b.x.real = width.x;
      curatt.pat_size.b.y.real = width.y;
   }
   else
   {
      curatt.pat_size.a.x.intr = (long) height.x;
      curatt.pat_size.a.y.intr = (long) height.y;
      curatt.pat_size.b.x.intr = (long) width.x;
      curatt.pat_size.b.y.intr = (long) width.y;
   }
   CGMout ( PATSIZE, pint, preal, str );
   return;
}

#ifdef PROTO
void m_colrtable ( Index index, int num, RGBcolour *col )
#else
void m_colrtable ( index, num, col )
Index index;
int num;
RGBcolour *col;
#endif
{
   long *pi = pint+1;
   register int i;

   if ( cgmstate != PIC_OPEN && cgmstate != MF_DEFAULTS )
   {
      CGMerror ("COLRTABLE", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   *pi++ = index;
   *pint = num;
   for ( i=0; i<num; i++ )
   {
      *pi++ = col->red;
      *pi++ = col->green;
      *pi++ = col->blue;
      col++;
   }

   CGMout ( COLRTABLE, pint, preal, str );
   return;
}

#ifdef PROTO
void m_asf ( int num, Code *aspect, Enum *value )
#else
void m_asf ( num, aspect, value )
int num;
Code *aspect;
Enum *value;
#endif
{
   Code c;
   long *pi = pint+1;
   register int i;

   if ( cgmstate != PIC_OPEN && cgmstate != MF_DEFAULTS )
   {
      CGMerror ("ASF", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   *pint = num;
   for ( i=0; i<num; i++ )
   {
      c = *aspect++;
      if ( c >= 0 && c <= 17 ) *pi++ = (Enum) c;
      else
         switch(c)
         {
            case LINETYPE:
            case LINEWIDTH:
            case LINECOLR:
               *pi++ = (Enum) (c - LINETYPE);
               break;
            case MARKERTYPE:
            case MARKERSIZE:
            case MARKERCOLR:
               *pi++ = (Enum) (c - MARKERTYPE + 3);
               break;
            case TEXTFONTINDEX:
            case TEXTPREC:
            case CHAREXPAN:
            case CHARSPACE:
            case TEXTCOLR:
               *pi++ = (Enum) (c - TEXTFONTINDEX + 6);
               break;
            case INTSTYLE:
            case FILLCOLR:
            case HATCHINDEX:
            case PATINDEX:
               *pi++ = (Enum) (c - INTSTYLE + 11);
               break;
            case EDGETYPE:
            case EDGEWIDTH:
            case EDGECOLR:
               *pi++ = (Enum) (c - EDGETYPE + 15);
               break;
            case ALL:
            case ALLLINE:
            case ALLMARKER:
            case ALLTEXT:
            case ALLFILL:
            case ALLEDGE:
               *pi++ = (Enum) c;
               break;
            default:
               (void) sprintf( mess, "(Code %d)", c);
               CGMerror ( "ASF", ERR_INVASF, ERROR, mess);
               return;
         }
      *pi++ = *value++;
   }

   CGMsetasf ( pint );
   CGMout ( ASF, pint, preal, str );
   return;
}

/************************************* Escape and External elements ****/

#ifdef PROTO
void m_escape ( int id, char *data )
#else
void m_escape ( id, data )
int id;
char *data;
#endif
{
   if ( cgmstate != PIC_OPEN && cgmstate != MF_DEFAULTS &&
        cgmstate != TEXT_OPEN && cgmstate != PIC_DESC )
   {
      CGMerror ("ESCAPE", ERR_STATE, FATAL, NULLSTR);
      return;
   }
   *pint = id;
   CGMout ( ESCAPE, pint, preal, data );
   return;
}

#ifdef PROTO
void m_message ( Enum action, char *message )
#else
void m_message ( action, message )
Enum action;
char *message;
#endif
{
   *pint = action;
   CGMout ( MESSAGE, pint, preal, message );
   return;
}

#ifdef PROTO
void m_appldata ( int id, char *data )
#else
void m_appldata ( id, data )
int id;
char *data;
#endif
{
   *pint = id;
   CGMout ( APPLDATA, pint, preal, data );
   return;
}
