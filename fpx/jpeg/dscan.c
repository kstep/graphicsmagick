/* -*-C-*-
******************************************************************************
*
* File:         dscan.c
* RCS:          $Header$
* Description:  Decode JPEG scan
* Author:       Hugh P. Nguyen
* Created:      Fri Dec  4 11:04:22 1992
* Modified:     Mon Jul 24 11:04:53 1995 by Gregory S. Yovanof
* Language:     C
* Package:      Hewlett-Packard JPEG Encoder/Decoder
*
* Copyright (c) 1999 Digital Imaging Group, Inc.
* For conditions of distribution and use, see copyright notice
* in Flashpix.h
*
******************************************************************************
*/

#include <stdlib.h>
#include <stdio.h>
#include "jpegconf.h"
#include "decoder.h"
#include "dbuffer.h"
#include "dectile.h"
#include "djpeg.h"
#include "dparser.h"
#include "winograd.h"
#include "dhuff.h"
#include "chen_dct.h"
#include "fpxmem.h"

#ifdef USE_ALL_TABLES
#define USE_SMALL_TABLES
#endif

#define DCT_BLOCK_SIZE 64

/* Type of 8x8 block passed to IDCT */
#define BLOCK_TYPE_0 0   /* DC != 0, the rest = 0 */
#define BLOCK_TYPE_1 1   /* Upper Left 4x4 != 0, the rest = 0 */
#define BLOCK_TYPE_2 2   /* other cases */

#ifdef USE_SMALL_TABLES
static int msb_mask[] = {         /* msb_mask[i] = 1 << (i-1) */
    0x0,  0x1,   0x2,   0x4,   0x8,   0x10,   0x20,   0x40,
    0x80, 0x100, 0x200, 0x400, 0x800, 0x1000, 0x2000, 0x4000
};
#endif

static int lowest_coef[] = {       /* lowest AC coef. given i */
    0, -1, -3, -7, -15, -31, -63, -127, -255, -511,
    -1023, -2047, -4095, -8191, -16383, -32767
};

static int izigzag_index[] =
{0,  1,  8, 16,  9,  2,  3, 10,
17, 24, 32, 25, 18, 11,  4,  5,
12, 19, 26, 33, 40, 48, 41, 34,
27, 20, 13,  6,  7, 14, 21, 28,
35, 42, 49, 56, 57, 50, 43, 36,
29, 22, 15, 23, 30, 37, 44, 51,
58, 59, 52, 45, 38, 31, 39, 46,
53, 60, 61, 54, 47, 55, 62, 63,
 0,  0,  0,  0,  0,  0,  0,  0,    /* extra 16 elements to catch */
 0,  0,  0,  0,  0,  0,  0,  0,    /*     errors in Decode_AC    */
};

void Clear_Old_DC(int *dp_last_dc)
{
    *dp_last_dc = *(dp_last_dc+1) = *(dp_last_dc+2) = *(dp_last_dc+3) = 0;
}

int Decode_DC(
  DB_STATE *db_state,
  DHUFF_TABLE *table,
  int comp,
  int *dp_last_dc)
{
    int s, diff;
    s = Decode_Huffman(db_state,table);
    
    if (s) {
#ifdef WARNING
        if ((s < 0) || (s > 15)) {
            warning(WARNING_DATA);
        }
#endif
        s &= 0xf;                           /* Make sure s is in [0,15] */
        if (((diff = DB_Get_Bits(db_state,s))        /* Get s bits */
#ifdef USE_SMALL_TABLES            
            & msb_mask[s]) == 0) {          /* MSB is 0 */
#else
            & (1 << (s-1))) == 0) {
#endif
            diff += lowest_coef[s];
        }
        
        /* Change the last DC */
        *(dp_last_dc+comp) += diff;
    }
    return(*(dp_last_dc+comp));
}

void Decode_AC(
  DB_STATE *db_state,
  DHUFF_TABLE *table,
  int *quant_vec,
  int *out)
{
    int *index;
    int r, s, n;
    
    for (index = out+1, r = 63; r > 0; r--) {   /* Clear buffer */
        *index++ = 0;
    }
    
    index = izigzag_index;                  /* Quantize DC coeff */
    out[*index++] = *out*(*quant_vec++);

    for (n = 63; n > 0; ) {                 /* process items 1 to 63 */
        s = Decode_Huffman(db_state,table);          /* Decode Huffman */
        r = (s >> 4) & 0xf;                 /* r = run-length */
        s &= 0xf;                           /* Find significant bits */
        if (s) {
            n -= r+1;
#ifdef WARNING
            if (n < 0) {
                warning(WARNING_DATA);
            }
#endif
            quant_vec += r;
            index += r;
            
            if (((r=DB_Get_Bits(db_state,s))         /* Get s bits */
#ifdef USE_SMALL_TABLES            
                & msb_mask[s]) == 0) {      /* MSB is 0 */
#else
                & (1 << (s-1))) == 0) {
#endif
                out[*index++] = (r+lowest_coef[s])*(*quant_vec++);
            } else {
                out[*index++] = r*(*quant_vec++);
            }
        } else if (r == 15) {               /* Zero run length code extnd */
            n -= 16;
#ifdef WARNING
            if (n < 0) {
                warning(WARNING_DATA);
            }
#endif
            quant_vec += 16;
            index += 16;
        } else {                            /* end-of-block (EOB) */
            break;                          /* break the for loop */
        }
    }
}

void Clear_Matrix(int *mat)
{
    int i;
    int *tmp;

    for (tmp = mat+1, i = 63; i > 0; i--) {
        *tmp++ = 0;
    }
}

void Decode_AC_Winograd(
  DB_STATE *db_state,
  DHUFF_TABLE *table,
  int *quant_vec,
  int *out)
{
    register int *index;
    register int r, s, n;
    
    Clear_Matrix(out);
    index = izigzag_index;
    /* Quantize DC coeff */
    out[*index++] = (int) SCALEQ(*out*(long)(*quant_vec++));

    for (n = 63; n > 0; ) {                 /* process items 1 to 63 */
        s = Decode_Huffman(db_state,table);          /* Decode Huffman */
        r = (s >> 4) & 0xf;                 /* r = run-length */
        s &= 0xf;                           /* Find significant bits */
        if (s) {
            n -= r+1;
#ifdef WARNING
            if (n < 0) {
                warning(WARNING_DATA);
            }
#endif
            quant_vec += r;
            index += r;
            
            if (((r=DB_Get_Bits(db_state,s))         /* Get s bits */
#ifdef USE_SMALL_TABLES            
                & msb_mask[s]) == 0) {      /* MSB is 0 */
#else
                & (1 << (s-1))) == 0) {
#endif
                out[*index++] = (int) SCALEQ((r+lowest_coef[s])*(long)(*quant_vec++));
            } else {
                out[*index++] = (int) SCALEQ(r*(long)(*quant_vec++));
            }
        } else if (r == 15) {               /* Zero run length code extnd */
            n -= 16;
#ifdef WARNING
            if (n < 0) {
                warning(WARNING_DATA);
            }
#endif
            quant_vec += 16;
            index += 16;
        } else {                            /* end-of-block (EOB) */
            break;                          /* break the for loop */
        }
    }
}


int Decode_AC_Pruned_Winograd(
  DB_STATE *db_state,
  DHUFF_TABLE *table,
  int *quant_vec,
  int *out)
{
    register int *index;
    register int r, s, n;
    
    index = izigzag_index;
    /* Quantize DC coeff */
    out[*index++] = (int) SCALEQ(*out*(long)(*quant_vec++));

    for (n = 63; n > 0; ) {                 /* process items 1 to 63 */
        s = Decode_Huffman(db_state,table);          /* Decode Huffman */
        r = (s >> 4) & 0xf;                 /* r = run-length */
        s &= 0xf;                           /* Find significant bits */
        if (s) {
            n -= r+1;
#ifdef WARNING
            if (n < 0) {
                warning(WARNING_DATA);
            }
#endif
            quant_vec += r;
            for (; r > 0; r--) out[*index++] = 0;
            
            if (((r=DB_Get_Bits(db_state,s))         /* Get s bits */
#ifdef USE_SMALL_TABLES            
                & msb_mask[s]) == 0) {      /* MSB is 0 */
#else
                & (1 << (s-1))) == 0) {
#endif
                out[*index++] = (int) SCALEQ((r+lowest_coef[s])*(long)(*quant_vec++));
            } else {
                out[*index++] = (int) SCALEQ(r*(long)(*quant_vec++));
            }
        } else if (r == 15) {               /* Zero run length code extnd */
            n -= 16;
#ifdef WARNING
            if (n < 0) {
                warning(WARNING_DATA);
            }
#endif
            quant_vec += 16;
            for (; r >= 0; r--) out[*index++] = 0;
        } else {                            /* end-of-block (EOB) */
            r = index - izigzag_index;
            if (r <= 1) {
                return(BLOCK_TYPE_0);
            } else if (r <= 10) {
                for (n -= 39; n > 0; n--) out[*index++] = 0;
                return(BLOCK_TYPE_1);
            } else if (r > 19) {
                for (; n > 0; n--) out[*index++] = 0;
                return(BLOCK_TYPE_2);
            } else {
                if (out[32] || out[4] || out[5] || out[12]) {
                    for (; n > 0; n--) out[*index++] = 0;
                    return(BLOCK_TYPE_2);
                } else {
                    for (n -= 39; n > 0; n--) out[*index++] = 0;
                    return(BLOCK_TYPE_1);
                }
            }

            break;
        }
    }
    return(BLOCK_TYPE_2);
}


void Decode_MCU_Chen(
  DB_STATE *db_state,
  SCAN *scan,
  int *dp_last_dc,
  int interleave) 
{
    int i, j;
    int *quant_table;
    int blockin[64];
    SCAN_COMPONENT *comp;

    if (scan->gray_scale) {
        comp = scan->comps;
        quant_table = comp->quant_table->elements;
        for (j = comp->hsampling*comp->vsampling; j > 0; j--) {
            *blockin = Decode_DC(db_state,comp->dc_table, comp->comp, dp_last_dc);
            Decode_AC(db_state,comp->ac_table, quant_table, blockin);
            IDct_Chen(db_state,blockin);
        }
        for (comp++, i = scan->ncomps-1; i > 0; i--, comp++) {
            quant_table = comp->quant_table->elements;
            for (j = comp->hsampling*comp->vsampling; j > 0; j--) {
                *blockin = Decode_DC(db_state,comp->dc_table, comp->comp, dp_last_dc);
                Decode_AC(db_state,comp->ac_table, quant_table, blockin);
            }
        }
    } else {
        for (comp = scan->comps, i = scan->ncomps; i > 0; i--, comp++) {
            quant_table = comp->quant_table->elements;
            for (j = comp->hsampling*comp->vsampling; j > 0; j--) {
                *blockin = Decode_DC(db_state,comp->dc_table, comp->comp, dp_last_dc);
                Decode_AC(db_state,comp->ac_table, quant_table, blockin);
              IDct_Chen(db_state,blockin);
            }
        }
    }
}


void Decode_MCU_Winograd(
  DB_STATE *db_state,
  SCAN *scan,
  int *dp_last_dc,
  int interleave)
{
    int i, j;
    int *quant_table;
    int block[64];
    SCAN_COMPONENT *comp;

    if (scan->gray_scale) {
        comp = scan->comps;
        quant_table = comp->quant_table->elements;
        for (j = comp->hsampling*comp->vsampling; j > 0; j--) {
            *block = Decode_DC(db_state,comp->dc_table, comp->comp, dp_last_dc);
            Decode_AC_Winograd(db_state,comp->ac_table, quant_table, block);

            DB_Setup_Data_Unit(db_state,comp->comp);
            IDct_Winograd(db_state,block);
        }
        for (comp++, i = scan->ncomps-1; i > 0; i--, comp++) {
            quant_table = comp->quant_table->elements;
            for (j = comp->hsampling*comp->vsampling; j > 0; j--) {
                *block = Decode_DC(db_state,comp->dc_table, comp->comp, dp_last_dc);
                Decode_AC_Winograd(db_state,comp->ac_table, quant_table, block);
            }
        }
    } else {
        for (comp = scan->comps, i = scan->ncomps; i > 0; i--, comp++) {
            quant_table = comp->quant_table->elements;
            for (j = comp->hsampling*comp->vsampling; j > 0; j--) {
                *block = Decode_DC(db_state,comp->dc_table, comp->comp, dp_last_dc);
                Decode_AC_Winograd(db_state,comp->ac_table, quant_table, block);

                DB_Setup_Data_Unit(db_state,comp->comp);
                IDct_Winograd(db_state,block);
            }
        }
    }
}


void Decode_MCU_Pruned_Winograd(
  DB_STATE *db_state,
  SCAN *scan,
  int *dp_last_dc,
  int interleave)
{
    int i, j;
    int *quant_table, block[64];
    SCAN_COMPONENT *comp;

    if (scan->gray_scale) {
        comp = scan->comps;
        quant_table = comp->quant_table->elements;
        for (j = comp->hsampling*comp->vsampling; j > 0; j--) {
            *block = Decode_DC(db_state,comp->dc_table, comp->comp, dp_last_dc);
            DB_Setup_Data_Unit(db_state,comp->comp);
            switch (Decode_AC_Pruned_Winograd(db_state,comp->ac_table,
                quant_table, block)) {
                case BLOCK_TYPE_0: {
                    IDct_DC_Winograd(db_state,block);
                    break;
                }
                case BLOCK_TYPE_1: {
                    IDct_Pruned_Winograd(db_state,block);
                    break;
                }
                default: {
                    IDct_Winograd(db_state,block);
                    break;
                }
            }
        }
        for (comp++, i = scan->ncomps-1; i > 0; i--, comp++) {
            quant_table = comp->quant_table->elements;
            for (j = comp->hsampling*comp->vsampling; j > 0; j--) {
                *block = Decode_DC(db_state,comp->dc_table, comp->comp, dp_last_dc);
                Decode_AC_Pruned_Winograd(db_state,comp->ac_table,quant_table, block);
            }
        }
    } else {
        for (comp = scan->comps, i = scan->ncomps; i > 0; i--, comp++) {
            quant_table = comp->quant_table->elements;
            for (j = comp->hsampling*comp->vsampling; j > 0; j--) {
                *block = Decode_DC(db_state,comp->dc_table, comp->comp, dp_last_dc);
                DB_Setup_Data_Unit(db_state,comp->comp);
                switch (Decode_AC_Pruned_Winograd(db_state,comp->ac_table,
                    quant_table, block)) {
                    case BLOCK_TYPE_0: {
                        IDct_DC_Winograd(db_state,block);
                        break;
                    }
                    case BLOCK_TYPE_1: {
                        IDct_Pruned_Winograd(db_state,block);
                        break;
                    }
                    default: {
                        IDct_Winograd(db_state,block);
                        break;
                    }
                }
            }
        }
    }
}


void Write_Blank_MCUs(
  DB_STATE *db_state,
  int nMCU,
  SCAN *scan,
  int interleave)
{
    int i, j, ncomps, bi;
    SCAN_COMPONENT *comp;

    ncomps = (scan->gray_scale) ? 1 : scan->ncomps;
    for (; nMCU > 0; nMCU--) {
        for (i = 0; i < ncomps; i++) {
            comp = scan->comps + i;
            for (j = comp->hsampling*comp->vsampling; j > 0; j--) {
           for (bi=64; bi > 0 ;bi--)
              *db_state->db_MCUptr++ = 0;
            }
        }
    }
}


/*
 * Return 0 if successful, or the MARKER found in the ECS.
 */
int Decode_Scan(
  DB_STATE *db_state,
  FRAME *frame,
  SCAN *scan,
  int method,      /* 0 = Chen, 1 = Winograd, 2 = Pruned Winograd */
  int interleave)
{
   register long nMCU;
  int *dp_last_dc;

  if ((dp_last_dc =(int *) FPX_malloc( 4 * sizeof(int)))==NULL) {
    return(ERROR_MEM);
  }
    
    Clear_Old_DC(dp_last_dc);
    if (scan->restart_interv) {
        long i;
        int j, nextRst;

        i = frame->totalMCU/scan->restart_interv;
        /* nMCU = number of MCU in the last ECS */
        nMCU = frame->totalMCU - i*scan->restart_interv;
        if (nMCU == 0) {
            i--;
            nMCU = scan->restart_interv;
        }
       
        nextRst = 0;
        for (; i > 0; i--) {
            if (method == 0) {
                for (j = scan->restart_interv; j > 0; j--) {
                    Decode_MCU_Chen(db_state,scan,dp_last_dc,interleave);
                }
            } else if (method == 1) {
                for (j = scan->restart_interv; j > 0; j--) {
                    Decode_MCU_Winograd(db_state,scan,dp_last_dc,interleave);
                }
            } else {
                for (j = scan->restart_interv; j > 0; j--) {
                    Decode_MCU_Pruned_Winograd(db_state,scan,dp_last_dc,interleave);
                }
            }
            DB_Align_Byte(db_state);
            Clear_Old_DC(dp_last_dc);

            /* Check Modulo # */
            if ((j = DP_Get_Next_Marker(db_state) - MARKER_RST0) != nextRst) {
                /* Missing some Data - Fill in missing MCUs with Blank MCUs */
                if ((j < 0) || (j > 7)) {     /* Not a RST Marker */
                    /* Fill the rest with blank MCUs,then return this marker*/
                    Write_Blank_MCUs(db_state,(int)(--i*scan->restart_interv+nMCU), scan,interleave);
                    i = 0;
                    return(j + MARKER_RST0);
                } else {
                    if (j > nextRst) {
                        j = j - nextRst;
                    } else {
                        j = j + 8 - nextRst;
                    }
                    nextRst = (nextRst + j + 1) % 8;
                    Write_Blank_MCUs(db_state, j*scan->restart_interv, scan,interleave);
                }
            } else {
#ifdef PRINT_INFO_MORE
                Print_Marker(j+MARKER_RST0);
#endif
                nextRst = (nextRst+1) % 8;
            }
        }
        if (method == 0) {
            for (; nMCU > 0; nMCU--) {
                Decode_MCU_Chen(db_state,scan,dp_last_dc,interleave);
            }
        } else if (method == 1) {
            for (; nMCU > 0; nMCU--) {
                Decode_MCU_Winograd(db_state,scan,dp_last_dc,interleave);
            }
        } else {
            for (; nMCU > 0; nMCU--) {
                Decode_MCU_Pruned_Winograd(db_state,scan,dp_last_dc,interleave);
            }
        }
    } else {
        if (method == 0) {
            for (nMCU = frame->totalMCU; nMCU > 0; nMCU--) {
                Decode_MCU_Chen(db_state,scan,dp_last_dc,interleave);
            }
        } else if (method == 1) {
            for (nMCU = frame->totalMCU; nMCU > 0; nMCU--) {
                Decode_MCU_Winograd(db_state,scan,dp_last_dc,interleave);
            }
        } else {
            for (nMCU = frame->totalMCU; nMCU > 0; nMCU--) {
                Decode_MCU_Pruned_Winograd(db_state,scan,dp_last_dc,interleave);
            }
        }
    }
    DB_Align_Byte(db_state);

  /* Write pixels stored in <db_state->db_MCUbuf> to <db_state->db_outbuf> */
  DB_Write_Scan_MCUs(db_state,frame->width,frame->height,interleave);

   if (dp_last_dc != NULL) FPX_free(dp_last_dc);
    return(0);
}
