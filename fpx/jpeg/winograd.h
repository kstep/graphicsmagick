/* -*-C-*-
******************************************************************************
*
* File:         winograd.h
* RCS:          $Header$
* Description:  contains Definitions for winograd.c
* Author:       Gregory S. Yovanof
* Created:      Fri Dec  4 11:04:22 1992
* Modified:     Fri Dec  4 11:04:53 1992
* Language:     C
* Package:      Hewlett-Packard JPEG Encoder/Decoder
*
* Copyright (c) 1999 Digital Imaging Group, Inc.
* For conditions of distribution and use, see copyright notice
* in Flashpix.h
*
******************************************************************************
*/

#ifdef P16BITS
#define LG2_DCT_SCALE 9
/***  9-bit Precision ***/
/* The Bs multiplication constants in the Inverse Winograd transform */
#define b1 724L
#define b2 1338L
#define b3 724L
#define b4 554L
#define b5 392L
#else
#define LG2_DCT_SCALE 15
/*** 15-bit Precision ***/
/* The Bs multiplication constants in the Inverse Winograd transform */
#define b1 46341L
#define b2 85627L
#define b3 46341L
#define b4 35468L
#define b5 25080L
#endif

#define INT32 long int
#define ONE ((INT32) 1)
#define DCT_SCALE (ONE << LG2_DCT_SCALE)
#define FIX(x) ((INT32) ((x) * DCT_SCALE + 0.5))
#define EXTRA_PRECISION    5 /** 5-extra bits of precision for the */
/**  input values to the inverse transform */
#define RIGHTS(r,s) ((r) >> (s)) /* Caution with rounding */
#define MID_PRECISION (LG2_DCT_SCALE-EXTRA_PRECISION) 
#define SCALEM(x) (RIGHTS(((x)+(ONE<<(LG2_DCT_SCALE-1))),LG2_DCT_SCALE))
/** scale the multiplied values on the compression side **/
#define SCALEQ(x) (RIGHTS(((x)+(ONE<<(MID_PRECISION-1))),MID_PRECISION))
/** input to the inverse DCT retains EXTRA_PRECISION bits **/
/** of precision to be removed at the last stage of the   **/
/** inverse transformation                                **/
#define SCALEE(x) \
  ( (int)(RIGHTS(((x)+(ONE<<(EXTRA_PRECISION-1))),EXTRA_PRECISION)) )
/** scale the <EXTRA>-extra bits of precision **/

JPEGEXPORT
void IDct_Winograd( DB_STATE *db_state, int *x);

JPEGEXPORT
void IDct_DC_Winograd(DB_STATE *db_state, int *x );

JPEGEXPORT
void IDct_Pruned_Winograd(DB_STATE *db_state, int *x);

JPEGEXPORT
void Fill_Winograd_Quant_Table2(int *in,  int *out);
