/*  RAL-CGM Interpreter header file: @(#) cgmlib.h  version 3.2
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
 * Description: User callable library for CGM calls
 *
 * Modification log:
 *
 *  16 Jun 90 RTPCreated
 *   2 Aug 91 RTP  Add SCCS id
 */

#ifndef CGMLIB_H
#define CGMLIB_H

/*  External variables */

#ifdef CGMLIB_C      /* Library entry */

#include "cgmpar.h"
#include "cgmin.h"

   extern FILE *cgmo;
   extern Enum cgmdriver;
   extern Logical cgmerrcount;

#define Extern    /*  routines are in this module */

#else             /*  User entry point */

#include "cgminit.h"

   FILE *cgmo, *cgmi = NULL;

#define Extern extern     /*  routines are extern to user program */

#endif


#ifdef PROTO
   Extern void cgminit ( Enum, char* );  /*  Initialise CGM output */
   Extern void CGMout  ( Code, long*, float*, char* );
   Extern int CGMerror ( char *, int, Enum, char * );
#else
   Extern void cgminit ();
   Extern void CGMout  ();
   Extern int CGMerror ();
#endif

/*  Define functions as 6 character names for portability */

#define  m_begmf           M_BGMF
#define  m_endmf           M_ENMF
#define  m_begpic          M_BGP
#define  m_begpicbody      M_BGPB
#define  m_endpic          M_ENDP
#define  m_mfversion       M_MFV
#define  m_mfdesc          M_MFD
#define  m_vdctype         M_VDC
#define  m_integerprec     M_IP
#define  m_realprec        M_RP
#define  m_indexprec       M_IXP
#define  m_colrprec        M_CP
#define  m_colrindexprec   M_CIXP
#define  m_maxcolrindex    M_MCI
#define  m_colrvalueext    M_CX
#define  m_mfelemlist      M_MFEL
#define  m_begmfdefaults   M_BMFD
#define  m_endmfdefaults   M_EMFD
#define  m_fontlist        M_FL
#define  m_charsetlist     M_CHS
#define  m_charcoding      M_CHCA
#define  m_scalemode       M_SCM
#define  m_colrmode        M_CSM
#define  m_linewidthmode   M_LNWS
#define  m_markersizemode  M_MKSS
#define  m_edgewidthmode   M_EDWS
#define  m_vdcext          M_VDCX
#define  m_backcolr        M_BC
#define  m_vdcintegerprec  M_VDCI
#define  m_vdcrealprec     M_VDCR
#define  m_auxcolr         M_XC
#define  m_transparency    M_TR
#define  m_cliprect        M_CLRC
#define  m_clip            M_CLIP
#define  m_line            M_PL
#define  m_disjtline       M_DJPL
#define  m_marker          M_PMK
#define  m_text            M_TX
#define  m_restrtext       M_RDTX
#define  m_apndtext        M_APTX
#define  m_polygon         M_PG
#define  m_polygonset      M_PGS
#define  m_cellarray       M_CA
#define  m_gdp             M_GDP
#define  m_rect            M_RC

#define  m_circle          M_CIR
#define  m_arc3pt          M_A3P
#define  m_arc3ptclose     M_A3PC
#define  m_arcctr          M_AC
#define  m_arcctrclose     M_ACC
#define  m_ellipse         M_EL
#define  m_elliparc        M_ELA
#define  m_elliparcclose   M_ELAC
#define  m_lineindex       M_LNBI
#define  m_linetype        M_LN
#define  m_linewidth       M_LNW
#define  m_linecolr        M_LNC
#define  m_markerindex     M_MKBI
#define  m_markertype      M_MK
#define  m_markersize      M_MKS
#define  m_markercolr      M_MKC
#define  m_textindex       M_TXBI
#define  m_textfontindex   M_TXFI
#define  m_textprec        M_TXP
#define  m_charexpan       M_CHXF
#define  m_charspace       M_CHSP
#define  m_textcolr        M_TXC
#define  m_charheight      M_CHH
#define  m_charori         M_CHOR
#define  m_textpath        M_TXPA
#define  m_textalign       M_TXAL
#define  m_charsetindex    M_CHSI
#define  m_altcharsetindex M_ACHS
#define  m_fillindex       M_FBI
#define  m_intstyle        M_IS
#define  m_fillcolr        M_FC
#define  m_hatchindex      M_HAI
#define  m_patindex        M_PAI
#define  m_edgeindex       M_EDBI
#define  m_edgetype        M_ED
#define  m_edgewidth       M_EDW
#define  m_edgecolr        M_EDC
#define  m_edgevis         M_EDV
#define  m_fillrefpt       M_FRFP
#define  m_pattable        M_PAT
#define  m_patsize         M_PAS
#define  m_colrtable       M_CT
#define  m_asf             M_ASF
#define  m_escape          M_ESC
#define  m_message         M_MESS
#define  m_appldata        M_AP

#define  CGMconvert_points CGMCONVP

/*  Metafile Delimiter elements */

#ifdef PROTO
Extern void  m_begmf ( char* );
Extern void  m_endmf ( void );
Extern void  m_begpic ( char* );
Extern void  m_begpicbody ( void );
Extern void  m_endpic ( void );

/*  Metafile Descriptor Elements */

Extern void  m_mfversion ( int );
Extern void  m_mfdesc ( char* );
Extern void  m_vdctype ( Enum );
Extern void  m_integerprec ( int );
Extern void  m_realprec ( int, int, int, Enum );
Extern void  m_indexprec ( int );
Extern void  m_colrprec ( int );
Extern void  m_colrindexprec ( int );
Extern void  m_maxcolrindex ( int );
Extern void  m_colrvalueext ( RGBcolour, RGBcolour );
Extern void  m_mfelemlist ( int, Code* );
Extern void  m_begmfdefaults ( void );
Extern void  m_endmfdefaults ( void );
Extern void  m_fontlist ( int, char** );
Extern void  m_charsetlist ( int, int*, char** );
Extern void  m_charcoding ( Enum );

/*  Picture Descriptor elements */

Extern void  m_scalemode ( Enum, float );
Extern void  m_colrmode ( Enum );
Extern void  m_linewidthmode ( Enum );
Extern void  m_markersizemode ( Enum );
Extern void  m_edgewidthmode ( Enum );
Extern void  m_vdcext ( Point, Point );
Extern void  m_backcolr ( RGBcolour );

/*  Control elements  */

Extern void  m_vdcintegerprec ( int );
Extern void  m_vdcrealprec ( int, int, int, Enum );
Extern void  m_auxcolr ( Colour );
Extern void  m_transparency ( Enum );
Extern void  m_cliprect ( Point, Point );
Extern void  m_clip ( Enum );

/*  Graphical Primitives */

Extern void  m_line ( int, Point* );
Extern void  m_disjtline ( int, Point* );
Extern void  m_marker ( int, Point* );
Extern void  m_text ( Point, Enum, char* );
Extern void  m_restrtext ( Point, Point, Enum, char* );
Extern void  m_apndtext ( Enum, char* );
Extern void  m_polygon ( int, Point* );
Extern void  m_polygonset ( int, Point*, Enum* );
Extern void  m_cellarray ( Point, Point, Point, int, int,
                           Indexcolour*, RGBcolour* );
Extern void  m_gdp ( int, int, Point*, char* );
Extern void  m_rect ( Point, Point );

Extern void  m_circle ( Point, Point, float );
Extern void  m_arc3pt ( Point, Point, Point );
Extern void  m_arc3ptclose ( Point, Point, Point, Enum );
Extern void  m_arcctr ( Point, Point, Point, float );
Extern void  m_arcctrclose ( Point, Point, Point, float, Enum );
Extern void  m_ellipse ( Point, Point, Point );
Extern void  m_elliparc ( Point, Point, Point, Point, Point );
Extern void  m_elliparcclose ( Point, Point, Point, Point, Point, Enum );

/*  Attributes */

Extern void  m_lineindex ( Index );
Extern void  m_linetype ( Index );
Extern void  m_linewidth ( float );
Extern void  m_linecolr ( Colour );
Extern void  m_markerindex ( Index );
Extern void  m_markertype ( Index );
Extern void  m_markersize ( float );
Extern void  m_markercolr ( Colour );
Extern void  m_textindex ( Index );
Extern void  m_textfontindex ( Index );
Extern void  m_textprec ( Enum );
Extern void  m_charexpan ( float );
Extern void  m_charspace ( float );
Extern void  m_textcolr ( Colour );
Extern void  m_charheight ( float );
Extern void  m_charori ( Vector, Vector );
Extern void  m_textpath ( Enum );
Extern void  m_textalign ( Enum, Enum, float, float );
Extern void  m_charsetindex ( Index );
Extern void  m_altcharsetindex ( Index );
Extern void  m_fillindex ( Index );
Extern void  m_intstyle ( Enum );
Extern void  m_fillcolr ( Colour );
Extern void  m_hatchindex ( Index );
Extern void  m_patindex ( Index );
Extern void  m_edgeindex ( Index );
Extern void  m_edgetype ( Index );
Extern void  m_edgewidth ( float );
Extern void  m_edgecolr ( Colour );
Extern void  m_edgevis ( Enum );
Extern void  m_fillrefpt ( Point );
Extern void  m_pattable ( Index, int, int, Prec, Colour* );
Extern void  m_patsize ( Vector, Vector );
Extern void  m_colrtable ( Index, int, RGBcolour* );
Extern void  m_asf ( int, Code*, Enum* );

/* Escape and External elements */

Extern void  m_escape ( int, char* );
Extern void  m_message ( Enum, char* );
Extern void  m_appldata ( int, char* );

#else   /* Non-ANSI C - no prototyping */

Extern void m_begmf(), m_endmf(), m_begpic(), m_begpicbody(), m_endpic();

/*  Metafile Descriptor Elements */

Extern void m_mfversion (), m_mfdesc(), m_vdctype(), m_integerprec (),
            m_realprec(), m_indexprec(), m_colrprec(), m_colrindexprec(),
            m_maxcolrindex(), m_colrvalueext(), m_mfelemlist(),
            m_begmfdefaults(), m_endmfdefaults(),
            m_fontlist(), m_charsetlist(), m_charcoding();

/*  Picture Descriptor elements */

Extern void m_scalemode(), m_colrmode(), m_linewidthmode(),
            m_markersizemode(), m_edgewidthmode(), m_vdcext(), m_backcolr();

/*  Control elements  */

Extern void m_vdcintegerprec(), m_vdcrealprec(), m_auxcolr(),
            m_transparency(), m_cliprect(), m_clip();

/*  Graphical Primitives */

Extern void m_line(), m_disjtline(), m_marker(), m_text(),
            m_restrtext(), m_apndtext(), m_polygon(), m_polygonset(),
            m_cellarray(), m_gdp(), m_rect(),
            m_circle(), m_arc3pt(), m_arc3ptclose(), m_arcctr(),
            m_arcctrclose(), m_ellipse(), m_elliparc(), m_elliparcclose();

/*  Attributes */

Extern void m_lineindex(), m_linetype(), m_linewidth(), m_linecolr(),
            m_markerindex(), m_markertype(), m_markersize(), m_markercolr(),
            m_textindex(), m_textfontindex(), m_textprec(),
            m_charexpan(), m_charspace(), m_textcolr(), m_charheight(),
            m_charori(), m_textpath(), m_textalign(),
            m_charsetindex(), m_altcharsetindex(),
            m_fillindex(), m_intstyle(), m_fillcolr(),
            m_hatchindex(), m_patindex(),
            m_edgeindex(), m_edgetype(), m_edgewidth(), m_edgecolr(),
            m_edgevis(), m_fillrefpt(),
            m_pattable(), m_patsize(), m_colrtable(), m_asf();

/*  Escape and External elements */

Extern void m_escape(), m_message(), m_appldata();

#endif

#undef Extern

#endif  /*  end of cgmlib.h */
