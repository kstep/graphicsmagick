/* -*-C-*-
******************************************************************************
*
* File:         chen_dct.c
* RCS:          $Header$
* Description:  Provides Chen's inverse DCT
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

#include <stdio.h>
#include "jpegconf.h"
#include "decoder.h"
#include "dbuffer.h"

/* Standard Macros */

#define LS(r,s) ((r) << (s))
#define RS(r,s) ((r) >> (s)) /* Caution with rounding... */

#define MSCALE(expr)  RS((expr),9)

#define SHIFT(val, expr)\
((((val = expr) < 0) ? (val-8) : (val+8)) / 16)

/* shift all elements 2 positions to the left, to get more accuracy when
   computing IDCT */
void Fill_Chen_Quant_Table(int *in,   /* regular quantization table */
int *out) /* chen quantization table */
{
  int i;
  for (i = 0; i < 64; i++) *out++ = LS(*in++, 2);
}

/* Cos constants */

#define c1d4 362L
#define c1d8 473L
#define c3d8 196L
#define c1d16 502L
#define c3d16 426L
#define c5d16 284L
#define c7d16 100L

/*
  VECTOR_DEFINITION makes the temporary variables vectors.
  Useful for machines with small register spaces.
*/

#ifdef VECTOR_DEFINITION
#define a0 a[0]
#define a1 a[1]
#define a2 a[2]
#define a3 a[3]
#define b0 b[0]
#define b1 b[1]
#define b2 b[2]
#define b3 b[3]
#define c0 c[0]
#define c1 c[1]
#define c2 c[2]
#define c3 c[3]
#endif

/*
 * Chen inverse dct.
*/
void IDct_Chen(DB_STATE *db_state,
int *x)
{
#ifdef VECTOR_DEFINITION
  register int a[4];
  register int b[4];
  register int c[4];
#else  
  register long int a0,a1,a2,a3;
  register long int b0,b1,b2,b3;
  register long int c0,c1,c2,c3;
  register int nif_a0;  /* same as a0, but an s int...used in SHIFT_AND_BOUND() macro */
#endif
  register int i;
  register int *aptr;

  for(aptr=x, i=0; i<8; i++, aptr++) {
    b0 = *aptr;
    a0 = aptr[8];
    b2 = aptr[16];
    a1 = aptr[24];
    b1 = aptr[32];
    a2 = aptr[40];
    b3 = aptr[48];
    a3 = aptr[56];

    /* Split into even mode  b0 = x0  b1 = x4  b2 = x2  b3 = x6.
            And the odd terms a0 = x1 a1 = x3 a2 = x5 a3 = x7.
         */
#ifdef TRUE_ISO
    c0 = MSCALE(c7d16*a0)-MSCALE(c1d16*a3);
    c1 = MSCALE(c3d16*a2)-MSCALE(c5d16*a1);
    c2 = MSCALE(c3d16*a1)+MSCALE(c5d16*a2);
    c3 = MSCALE(c1d16*a0)+MSCALE(c7d16*a3);
#else
    c0 = MSCALE((c7d16*a0)-(c1d16*a3));
    c1 = MSCALE((c3d16*a2)-(c5d16*a1));
    c2 = MSCALE((c3d16*a1)+(c5d16*a2));
    c3 = MSCALE((c1d16*a0)+(c7d16*a3));
#endif

    /* First Butterfly on even terms.*/

    a0 = MSCALE(c1d4*(b0+b1));
    a1 = MSCALE(c1d4*(b0-b1));

#ifdef TRUE_ISO
    a2 = MSCALE(c3d8*b2)-MSCALE(c1d8*b3);
    a3 = MSCALE(c1d8*b2)+MSCALE(c3d8*b3);
#else
    a2 = MSCALE((c3d8*b2)-(c1d8*b3));
    a3 = MSCALE((c1d8*b2)+(c3d8*b3));
#endif

    b0 = a0+a3;
    b1 = a1+a2;
    b2 = a1-a2;
    b3 = a0-a3;

    /* Second Butterfly */

    a0 = c0+c1;
    a1 = c0-c1;
    a2 = c3-c2;
    a3 = c3+c2;

    c0 = a0;
    c1 = MSCALE(c1d4*(a2-a1));
    c2 = MSCALE(c1d4*(a2+a1));
    c3 = a3;

    *aptr = (int)(b0+c3);
    aptr[8] = (int)(b1+c2);
    aptr[16] = (int)(b2+c1);
    aptr[24] = (int)(b3+c0);
    aptr[32] = (int)(b3-c0);
    aptr[40] = (int)(b2-c1);
    aptr[48] = (int)(b1-c2);
    aptr[56] = (int)(b0-c3);
  }

  /* Loop over rows */

  for(aptr = x, i=0; i<8; i++) {
    b0 = *(aptr++);
    a0 = *(aptr++);
    b2 = *(aptr++);
    a1 = *(aptr++);
    b1 = *(aptr++);
    a2 = *(aptr++);
    b3 = *(aptr++);
    a3 = *(aptr++);

    /*
       Split into even mode  b0 = x0  b1 = x4  b2 = x2  b3 = x6.
       And the odd terms a0 = x1 a1 = x3 a2 = x5 a3 = x7.
      */

#ifdef TRUE_ISO
    c0 = MSCALE(c7d16*a0)-MSCALE(c1d16*a3);
    c1 = MSCALE(c3d16*a2)-MSCALE(c5d16*a1);
    c2 = MSCALE(c3d16*a1)+MSCALE(c5d16*a2);
    c3 = MSCALE(c1d16*a0)+MSCALE(c7d16*a3);
#else
    c0 = MSCALE((c7d16*a0)-(c1d16*a3));
    c1 = MSCALE((c3d16*a2)-(c5d16*a1));
    c2 = MSCALE((c3d16*a1)+(c5d16*a2));
    c3 = MSCALE((c1d16*a0)+(c7d16*a3));
#endif

    /* First Butterfly on even terms.*/

    a0 = MSCALE(c1d4*(b0+b1));
    a1 = MSCALE(c1d4*(b0-b1));

#ifdef TRUE_ISO
    a2 = MSCALE(c3d8*b2)-MSCALE(c1d8*b3);
    a3 = MSCALE(c1d8*b2)+MSCALE(c3d8*b3);
#else
    a2 = MSCALE((c3d8*b2)-(c1d8*b3));
    a3 = MSCALE((c1d8*b2)+(c3d8*b3));
#endif

    /* Calculate last set of b's */

    b0 = a0+a3;
    b1 = a1+a2;
    b2 = a1-a2;
    b3 = a0-a3;

    /* Second Butterfly */

    a0 = c0+c1;
    a1 = c0-c1;
    a2 = c3-c2;
    a3 = c3+c2;

    c0 = a0;
    c1 = MSCALE(c1d4*(a2-a1));
    c2 = MSCALE(c1d4*(a2+a1));
    c3 = a3;

    /*
       Retrieve correct accuracy. We have additional factor
       of 16 that must be removed. Use SHIFT.
      */

    /* change these casts to BYTE16 or whatever the approved FlashPix ones are...*/
    nif_a0 = a0;
    SHIFT_AND_BOUND(nif_a0, (int)SHIFT(a0, b0+c3), *db_state->db_MCUptr++);
    SHIFT_AND_BOUND(nif_a0, (int)SHIFT(a0, b1+c2), *db_state->db_MCUptr++);
    SHIFT_AND_BOUND(nif_a0, (int)SHIFT(a0, b2+c1), *db_state->db_MCUptr++);
    SHIFT_AND_BOUND(nif_a0, (int)SHIFT(a0, b3+c0), *db_state->db_MCUptr++);
    SHIFT_AND_BOUND(nif_a0, (int)SHIFT(a0, b3-c0), *db_state->db_MCUptr++);
    SHIFT_AND_BOUND(nif_a0, (int)SHIFT(a0, b2-c1), *db_state->db_MCUptr++);
    SHIFT_AND_BOUND(nif_a0, (int)SHIFT(a0, b1-c2), *db_state->db_MCUptr++);
    SHIFT_AND_BOUND(nif_a0, (int)SHIFT(a0, b0-c3), *db_state->db_MCUptr++);
  }
}
