/* -*-C-*-
******************************************************************************
*
* File:         dct.h
* RCS:          $Header$
* Description:  see win_dct.c
* Author:       Gregory S. Yovanof
* Created:      Fri Dec  4 11:04:22 1992
* Initial Source Release:     Wed Jan 17 1996
* Language:     C
* Package:      Hewlett-Packard JPEG Encoder/Decoder
*
* Copyright (c) 1999 Digital Imaging Group, Inc.
* For conditions of distribution and use, see copyright notice
* in Flashpix.h
*
******************************************************************************
*/

#define INT32 long int
#define ONE ((INT32) 1)
#define LG2_DCT_SCALE 15
#define DCT_SCALE (ONE << LG2_DCT_SCALE)
#define FIX(x) ((INT32) ((x) * DCT_SCALE + 0.5))
#define RIGHTS(r,s) ((r) >> (s)) /* Caution with rounding */
#define SCALEM(x) (RIGHTS(((x)+(ONE<<(LG2_DCT_SCALE-1))),LG2_DCT_SCALE))
/* scale the multiplied values on the compression side */

/* As Table */
/* The As multiplication constants in the Winograd transform */
/*** 15-bit Precision ***/
#define a1 23170L 
#define a2 17734L
#define a3 23170L
#define a4 42813L
#define a5 12540L

/* Bs Table */
/*** 15-bit Precision ***/
#define b1 46341L  
#define b2 85627L
#define b3 46341L
#define b4 35468L
#define b5 25080L

