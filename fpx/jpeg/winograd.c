/* -*-C-*-
******************************************************************************
*
* File:         winograd.c
* RCS:          $Header$
* Description:  provides Winograd's inverse DCT
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
#include <math.h>
#include "jpegconf.h"
#include "decoder.h"
#include "dbuffer.h"
#include "winograd.h"

/*BFUNC 
Fill_Winograd_Quant_Table2() multiplies all the components of the Quantization
matrix by the fractional normalization constants as required by the Winograd
Transform and then it converts the resulting fractional numbers into integers
by multiplying them by the DCT_SCALE (thus retaining
LG2_DCT_SCALE bits of precision). It  returns the matrix created.
EFUNC*/

static double FloatI2DDctNorm[] = {
  0.125000,0.173380,0.173380,0.163320,0.240485,0.163320,0.146984,0.226532,
  0.226532,0.146984,0.125000,0.203873,0.213388,0.203873,0.125000,0.098212,
  0.173380,0.192044,0.192044,0.173380,0.098212,0.067650,0.136224,0.163320,
  0.172835,0.163320,0.136224,0.067650,0.034566,0.093833,0.128320,0.146984,
  0.146984,0.128320,0.093833,0.034566,0.047944,0.088388,0.115485,0.125000,
  0.115485,0.088388,0.047944,0.045162,0.079547,0.098212,0.098212,0.079547,
  0.045162,0.040645,0.067650,0.077165,0.067650,0.040645,0.034566,0.053152,
  0.053152,0.034566,0.027158,0.036612,0.027158,0.018707,0.018707,0.009558
};

void Fill_Winograd_Quant_Table2(
int *in,                         /* regular quantization table */
int *out)                        /* winograd quantization table */
{
  double *fptr;
  int i;

  fptr = FloatI2DDctNorm;
  for (i = 0; i < 64; i++) {
    *out++ = FIX(((double) *in++) * (*fptr++));
  }
}


/*
*********************************************************************
winograd_dct.c
This is the IDCT algorithm  based on the small FFT Winograd transform
from Trans. IEICE, vol. E 71(11), 1095-1097, Nov. 1988
*********************************************************************
*/

/* Associate dummy variables with registers */
#define in0 r0
#define in1 r1
#define in2 r0
#define in3 r1
#define in4 r0
#define in5 r0
#define in6 r1
#define in7 r1

#define stg12 tmp
#define stg13 r0
#define stg4 r2
#define stg5 r4
#define stg6 r0
#define stg7 r3

#define stg15 r1
#define stg17 r4
#define hold1 r2
#define hold2 r0
#define hold3 r1
#define hold4 r5
#define hold5 r6

#define stg22 r1
#define stg24 r2
#define stg25 r1
#define stg26 r0

#define stg30 r2
#define stg31 r3
#define stg32 r1

#define stg40 tmp
#define stg41 r2
#define stg42 r1
#define stg43 r0
#define stg44 r7

/*BFUNC
IDct_Winograd is implemented according to the inverse dct flow diagram in
the IEICE paper. It takes two input arrays that must be defined before the call.
EFUNC*/

void IDct_Winograd( DB_STATE *db_state, int *x)
{
  register int *lptr, r0,r1,r2,r3,r4,r5,r6,r7, tmp, i;

  /** Do columns **/
  for(lptr=x, i = 8; i > 0; i--, lptr++) {
    in4 = lptr[40];
    in7 = lptr[24];
    stg4  = in4 - in7;
    stg7  = in4 + in7;

    in5 = lptr[8];
    in6 = lptr[56];
    stg5  = in5 + in6;
    stg6  = in5 - in6;

    hold3 = SCALEM(b5*(stg4-stg6));
    stg26 = SCALEM(b4*stg6) - hold3;

    stg24 = hold3 - SCALEM(b2*stg4);

    stg15 = stg5 - stg7;
    stg17 = stg5 + stg7;

    hold4 = stg26 - stg17;
    hold5 = SCALEM(b3*stg15) - hold4;
    stg44 = hold5 + stg24;

    in0 = lptr[0];
    in1 = lptr[32];
    stg30 = in0 + in1;
    stg31 = in0 - in1;

    in2 = lptr[16];
    in3 = lptr[48];
    stg12 = in2 - in3;
    stg13 = in2 + in3;

    stg32 = SCALEM(b1*stg12) - stg13;

    stg40 = stg30 + stg13;
    stg43 = stg30 - stg13;
    stg41 = stg31 + stg32;
    stg42 = stg31 - stg32;

    lptr[0]  = (stg40 + stg17);
    lptr[8]  = (stg41 + hold4);
    lptr[16] = (stg42 + hold5);
    lptr[24] = (stg43 - stg44);
    lptr[32] = (stg43 + stg44);
    lptr[40] = (stg42 - hold5);
    lptr[48] = (stg41 - hold4);
    lptr[56] = (stg40 - stg17);
  }

  /*  Do Rows */
  for(lptr=x,i = 8; i > 0; i--, lptr+=8) {
    in4 = lptr[5];
    in7 = lptr[3];
    stg4  = in4 - in7;
    stg7  = in4 + in7;

    in5 = lptr[1];
    in6 = lptr[7];
    stg5  = in5 + in6;
    stg6  = in5 - in6;

    hold3 = SCALEM(b5*(stg4-stg6));
    stg26 = SCALEM(b4*stg6) - hold3;

    stg24 = hold3 - SCALEM(b2*stg4);

    stg15 = stg5 - stg7;
    stg17 = stg5 + stg7;

    hold4 = stg26 - stg17;
    hold5 = SCALEM(b3*stg15) - hold4;
    stg44 = hold5 + stg24;

    in0 = lptr[0];
    in1 = lptr[4];
    stg30 = in0 + in1;
    stg31 = in0 - in1;

    in2 = lptr[2];
    in3 = lptr[6];
    stg12 = in2 - in3;
    stg13 = in2 + in3;

    stg32 = SCALEM(b1*stg12) - stg13;

    stg40 = stg30 + stg13;
    stg43 = stg30 - stg13;
    stg41 = stg31 + stg32;
    stg42 = stg31 - stg32;

    /* remove the 5-EXTRA-fractional bits by using SCALEE */
    SHIFT_AND_BOUND(r3,SCALEE(stg40 + stg17),*db_state->db_MCUptr++);
    SHIFT_AND_BOUND(r3,SCALEE(stg41 + hold4),*db_state->db_MCUptr++);
    SHIFT_AND_BOUND(r3,SCALEE(stg42 + hold5),*db_state->db_MCUptr++);
    SHIFT_AND_BOUND(r3,SCALEE(stg43 - stg44),*db_state->db_MCUptr++);
    SHIFT_AND_BOUND(r3,SCALEE(stg43 + stg44),*db_state->db_MCUptr++);
    SHIFT_AND_BOUND(r3,SCALEE(stg42 - hold5),*db_state->db_MCUptr++);
    SHIFT_AND_BOUND(r3,SCALEE(stg41 - hold4),*db_state->db_MCUptr++);
    SHIFT_AND_BOUND(r3,SCALEE(stg40 - stg17),*db_state->db_MCUptr++);
  }
}


#undef in0
#undef in1
#undef in2
#undef in3
#undef in4
#undef in5
#undef in6
#undef in7

#undef stg12
#undef stg13
#undef stg4
#undef stg5
#undef stg6
#undef stg7

#undef stg15
#undef stg17
#undef hold1
#undef hold2
#undef hold3
#undef hold4
#undef hold5

#undef stg22
#undef stg24
#undef stg25
#undef stg26

#undef stg30
#undef stg31
#undef stg32

#undef stg40
#undef stg41
#undef stg42
#undef stg43
#undef stg44
#undef stg45
#undef stg46
#undef stg47

/* Associate dummy variables with registers */

#define stg12 r1
#define stg5 r1
#define stg7 r0

#define stg15 r2
#define stg17 r4
#define hold3 r3
#define hold4 r5
#define hold5 r6

#define stg24 r0
#define stg26 r1

#define stg30 r0
#define stg32 r1

#define stg40 r2
#define stg41 tmp
#define stg42 r0
#define stg43 r3
#define stg44 r7

/*BFUNC
IDct_Winograd is implemented according to the inverse dct flow diagram in
the IEICE paper. It takes two input arrays that must be defined before the call.
EFUNC*/

void IDct_Pruned_Winograd( DB_STATE *db_state, int *x)
{
  register int *lptr, r0,r1,r2,r3,r4,r5,r6,r7, tmp, i;

  /** Do columns **/
  for(lptr=x, i = 4; i > 0; i--, lptr++) {
    stg7 = -lptr[24];
    stg5 = lptr[8];

    stg15 = stg7 + stg5;
    stg17 = stg7 - stg5;

    hold3 = SCALEM(b5*stg17);
    stg26 = SCALEM(b4*stg5) - hold3;
    stg24 = hold3 - SCALEM(b2*stg7);

    hold4 = stg26 + stg17;
    hold5 = SCALEM(b3*stg15) - hold4;
    stg44 = hold5 + stg24;

    stg30 = lptr[0];
    stg12 = lptr[16];
    stg40 = stg30 + stg12;
    stg43 = stg30 - stg12;

    stg32 = SCALEM(b1*stg12) - stg12;
    stg41 = stg30 + stg32;
    stg42 = stg30 - stg32;

    lptr[0]  = (stg40 - stg17);
    lptr[8]  = (stg41 + hold4);
    lptr[16] = (stg42 + hold5);
    lptr[24] = (stg43 - stg44);
    lptr[32] = (stg43 + stg44);
    lptr[40] = (stg42 - hold5);
    lptr[48] = (stg41 - hold4);
    lptr[56] = (stg40 + stg17);
  }

  /*  Do Rows */
  for(lptr=x,i = 8; i > 0; i--, lptr+=8) {
    stg7 = -lptr[3];
    stg5 = lptr[1];

    stg15 = stg7 + stg5;
    stg17 = stg7 - stg5;

    hold3 = SCALEM(b5*stg17);
    stg26 = SCALEM(b4*stg5) - hold3;
    stg24 = hold3 - SCALEM(b2*stg7);

    hold4 = stg26 + stg17;
    hold5 = SCALEM(b3*stg15) - hold4;
    stg44 = hold5 + stg24;

    stg30 = lptr[0];
    stg12 = lptr[2];
    stg40 = stg30 + stg12;
    stg43 = stg30 - stg12;

    stg32 = SCALEM(b1*stg12) - stg12;
    stg41 = stg30 + stg32;
    stg42 = stg30 - stg32;

    /* remove the 5-EXTRA-fractional bits by using SCALEE */
    SHIFT_AND_BOUND(r3,SCALEE(stg40 - stg17),*db_state->db_MCUptr++);
    SHIFT_AND_BOUND(r3,SCALEE(stg41 + hold4),*db_state->db_MCUptr++);
    SHIFT_AND_BOUND(r3,SCALEE(stg42 + hold5),*db_state->db_MCUptr++);
    SHIFT_AND_BOUND(r3,SCALEE(stg43 - stg44),*db_state->db_MCUptr++);
    SHIFT_AND_BOUND(r3,SCALEE(stg43 + stg44),*db_state->db_MCUptr++);
    SHIFT_AND_BOUND(r3,SCALEE(stg42 - hold5),*db_state->db_MCUptr++);
    SHIFT_AND_BOUND(r3,SCALEE(stg41 - hold4),*db_state->db_MCUptr++);
    SHIFT_AND_BOUND(r3,SCALEE(stg40 + stg17),*db_state->db_MCUptr++);
  }
}


/*** DC-component only  ***/
void IDct_DC_Winograd( DB_STATE *db_state, int *x )
{
  register int i,tmp;

  for (i=64; i > 0 ;i--)
    SHIFT_AND_BOUND(tmp,SCALEE(*x),*db_state->db_MCUptr++);
}
