/* -*-C-*-
*************************************************************************
*
* File:         win_dct.c
* RCS:          $Header$
* Description:  Forward DCT based on Winograd's algorithm
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

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "jpegconf.h"
#include "dct.h"

/* DCT-Scale Factors -- This table has been zigzagged */
static double Float2DDctNorm[] = {
  0.125000, 0.090120, 0.090120, 0.095671, 0.064973, 0.095671, 0.106304, 0.068975,
  0.068975, 0.106304, 0.125000, 0.076641, 0.073223, 0.076641, 0.125000, 0.159095,
  0.090120, 0.081361, 0.081361, 0.090120, 0.159095, 0.230970, 0.114701, 0.095671,
  0.090404, 0.095671, 0.114701, 0.230970, 0.453064, 0.166520, 0.121766, 0.106304,
  0.106304, 0.121766, 0.166520, 0.453064, 0.326641, 0.176777, 0.135299, 0.125000,
  0.135299, 0.176777, 0.326641, 0.346760, 0.196424, 0.159095, 0.159095, 0.196424,
  0.346760, 0.385299, 0.230970, 0.202489, 0.230970, 0.385299, 0.453064, 0.293969,
  0.293969, 0.453064, 0.576641, 0.426777, 0.576641, 0.837153, 0.837153, 1.642134
};

void  Fill_Winograd_Quant_Table(int *in,  /* regular quantization table */
int *out) /* winograd quantization table */
{
  double *fptr;
  int i, q;

#ifdef DEBUG
  if (((-1L) >> 15) >= 0) {
    debug("Fill_Winograd_Quant_Table: This compiler does not shift signed bit");
  }
#endif

  /*
     * Note that in order for FIX(fptr[i]/in[i]) to fit inside 16-bit signed
     * int, (fptr[i]/in[i] < 1) => (in[i] > fptr[i]) (1). Since in[i] >= 1,
     * and (fptr[i] < 1) for i = 0 to 62, (1) is true for i = 0 to 62. For
     * i = 63, (1) is true only when in[63] >= 2
     */
  if (in[63] < 2) in[63] = 2;

  fptr = Float2DDctNorm;
  for (i = 0; i < 64; i++) {
    *out++ = q = FIX((*fptr++) / ((float) *in++));
  }
}

/* Associate dummy variables with registers */
#define in0 r0
#define in1 r0
#define in2 r0
#define in3 r0
#define in4 r2
#define in5 r4
#define in6 r1
#define in7 r1

#define stg0 r1
#define stg1 r1
#define stg2 r0
#define stg3 r0
#define stg4 tmp
#define stg5 r7
#define stg6 r6
#define stg7 r5

#define stg10 r3
#define stg11 r0
#define stg12 r4
#define stg13 r2
#define stg14 r0
#define stg15 r2
#define stg16 r1

#define stg23 r0

#define stg32 r0
#define stg34 r0
#define stg35 r2
#define stg36 r1

#define stg45 r3
#define stg47 r2

#define hold1 r0
#define hold2 r1
#define hold3 r3

void Dct(int *x)
{
  register int *jptr, r0,r1,r2,r3,r4,r5,r6,r7, tmp, i;

  /* Do Rows */
  for (jptr = x, i = 8; i > 0; i--, jptr += 8) {
    in0 = jptr[0];
    in7 = jptr[7];
    stg7 = in0 - in7;
    stg0 = in0 + in7;

    in3 = jptr[3];
    in4 = jptr[4];
    stg4 = in3 - in4;
    stg3 = in3 + in4;

    stg10 = stg0 + stg3;
    stg13 = stg0 - stg3;

    in1 = jptr[1];
    in6 = jptr[6];
    stg6 = in1 - in6;
    stg1 = in1 + in6;

    in2 = jptr[2];
    in5 = jptr[5];
    stg5 = in2 - in5;
    stg2 = in2 + in5;

    stg12 = stg1 - stg2;
    stg11 = stg1 + stg2;

    jptr[0] = stg10 + stg11;
    jptr[4] = stg10 - stg11;

    stg23 = stg12 + stg13;
    stg32 = SCALEM(a1*stg23);
    jptr[2] = stg32 + stg13;
    jptr[6] = stg13 - stg32;

    stg14 = - stg4 - stg5;
    stg15 = stg5 + stg6;
    stg35 = SCALEM(a3*stg15);

    stg16 = stg6 + stg7;
    hold3 = SCALEM(a4*stg16);
    hold2 = SCALEM(a5*(stg14+stg16));
    hold1 = SCALEM(a2*stg14);

    stg34 = -hold1 - hold2;
    stg36 = hold3 - hold2;

    stg45 = stg35 + stg7;
    stg47 = stg7  - stg35;

    jptr[5] = stg47 + stg34;
    jptr[1] = stg45 + stg36;
    jptr[7] = stg45 - stg36;
    jptr[3] = -stg34 + stg47;
  }

  /** Do Columns **/
  for (jptr = x, i = 8; i > 0; i--, jptr++) {
    in0 = jptr[0];
    in7 = jptr[56];
    stg7 = in0 - in7;
    stg0 = in0 + in7;

    in3 = jptr[24];
    in4 = jptr[32];
    stg4 = in3 - in4;
    stg3 = in3 + in4;

    stg10 = stg0 + stg3;
    stg13 = stg0 - stg3;

    in1 = jptr[8];
    in6 = jptr[48];
    stg6 = in1 - in6;
    stg1 = in1 + in6;

    in2 = jptr[16];
    in5 = jptr[40];
    stg5 = in2 - in5;
    stg2 = in2 + in5;

    stg12 = stg1 - stg2;
    stg11 = stg1 + stg2;

    jptr[0] = stg10 + stg11;
    jptr[32] = stg10 - stg11;

    stg23 = stg12 + stg13;
    stg32 = SCALEM(a1*stg23);
    jptr[16] = stg32 + stg13;
    jptr[48] = stg13 - stg32;

    stg14 = - stg4 - stg5;
    stg15 = stg5 + stg6;
    stg35 = SCALEM(a3*stg15);

    stg16 = stg6 + stg7;
    hold3 = SCALEM(a4*stg16);
    hold2 = SCALEM(a5*(stg14+stg16));
    hold1 = SCALEM(a2*stg14);

    stg34 = -hold1 - hold2;
    stg36 = hold3 - hold2;

    stg45 = stg35 + stg7;
    stg47 = stg7  - stg35;

    jptr[40] = stg47 + stg34;
    jptr[8] = stg45 + stg36;
    jptr[56] = stg45 - stg36;
    jptr[24] = -stg34 + stg47;
  }

}
