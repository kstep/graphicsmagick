/*  RAL-CGM Interpreter header file: @(#) cgmmap.h  version 3.1
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
 * Description: Mapping of variables and functions to shorter names
 *
 * Modification log:
 *
 *   2 Aug 91 RTP  Add SCCS id
 */

#ifndef CGMMAP_H
#define CGMMAP_H

/*  CGM External variables */

#define  cgmerrcount    CGMERC
#define  cgmerr         CGMERF
#define  cgmversion     CGMVER
#define  cgmverno       CGMVNR
#define  cgmelement     CGMELE
#define  cgmralgks      CGMGKS
#define  cgmverbose     CGMVBS
#define  cgmdriver      CGMDRI
#define  cgmrandom      CGMRAN
#define  cgmpresent     CGMPRE
#define  cgmwindow      CGMWIN

#ifdef EBCDIC
#define  cgmebcdic      CGMEBC
#define  cgminnative    CGMINN
#endif

#define  miccharsub     MICCHA
#define  micsubchar     MICSUB
#define  commondef      COMDEF

/*  CGM Functions */

#define  CGMerror       CGMERR
#define  CGMoutput      CGMOPT
#define  CGMprofile     CGMPRO
#define  CGMdefaults    CGMDEF
#define  CGMialloc      CGMIAL
#define  CGMstframe     CGMSTF
#define  CGMralloc      CGMRAL
#define  CGMsalloc      CGMSAL
#define  CGMsetasf      CGMSET
#define  CGMgetfname    CGMGET
#define  CGMmodfname    CGMMFN
#define  CGMmodfextn    CGMMFE

/*  CGM Utilities */

#define  CGMfopen       CGMFOP
#define  CGMpath        CGMPAT
#define  CGMframe       CGMFRA
#define  CGMconvex      CGMCVX
#define  ATTmarker      ATTMAR
#define  ATTedge        ATTEDG
#define  ATTfill        ATTFIL
#define  ATTline        ATTLIN
#define  ATTtext        ATTTXT
#define  FILpolycheck   FILPOL
#define  FILarea        FILARE
#define  GDPcentre      GDPCEN
#define  GDPcircle      GDPCIR
#define  GDPellipse     GDPELL
#define  GRAinsect      GRAINS
#define  TXTaccinf      TXTACC
#define  TXTalign       TXTALI
#define  TXTfree        TXTFRE
#define  TXTxform       TXTXFO
#define  TXTgheight     TXTGHE
#define  TXTgwidth      TXTGWI
#define  BEZgetfd       BEZGFD
#define  BEZtext        BEZTXT
#define  HERgetfd       HERGFD
#define  HERtext        HERTXT
#define  FNTclist       FNTCLI
#define  FNTclose       FNTCLO
#define  FNTflist       FNTFLI
#define  FNTinit        FNTINI
#define  FNTmatch       FNTMAT

/*  Workstation driver functions */
#define  CGMIbin        CGMIBI
#define  CGMIchar       CGMICH
#define  CGMItext       CGMITX
#define  CGMObin        CGMOBI
#define  CGMOchar       CGMOCH
#define  CGMOtext       CGMOTX
#define  CGMocode       CGMOCD
#define  CGMOigl        CGMOIG

#define  Xw_getpix      XWGETP
#define  Xw_options     XWOPTS

#define  PSoptions      PSOPTS
#define  PSBEZtext      PSBEZT
#define  PSHERtext      PSHERT
#define  PSgetfd        PSGETF

#define  IGLoptions     IGLOPT
#define  IGLedge        IGLEDG
#define  IGLfill        IGLFIL
#define  IGLtxline      IGLTXL
#define  IGLtxfill      IGLTXF

/*  RALCGM external variables */
#define cgmabort CGMABO
#define cgmascii CGMASC
#define cgmcsets CGMCSE
#define cgmerrs  CGMERS
#define cgmfonts CGMFNT
#define cgminput CGMINP
#define cgmlist  CGMLST
#define cgmmfile CGMMFI
#define cgmnext  CGMNXT
#define cgmocode CGMOCD
#define cgmofile CGMOFI
#define cgmroot  CGMROO
#define cgmstate CGMSTA
#define cgmterm  CGMTRM
#define cgmwvrat CGMWVR

#define chardef  CHRDEF
#define curchar  CUCHAR
#define curibin  CURIBI
#define curichar CURICH
#define curitext CURITX
#define curtext  CURTXT
#define drivers  DRVRS
#define mfichar  MFICHR
#define mfitext  MFITXT
#define oldchar  OLDCHR
#define st_start ST_STT
#define st_term  ST_TRM
#define textdef  TXTDEF
#define ftnames  FTNAMS

#endif  /*  end of cgmmap.h */
