/* -*-C-*-
******************************************************************************
*
* File:         enctile.c
* RCS:          $Header$
* Description:  JPEG encodes a tile of data
* Author:       Gregory S. Yovanof
* Created:      Tue Jun 13 11:04:22 1995
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

#include <stdlib.h>
#include <stdio.h>
#include "jpegconf.h"
#include "enctile.h"
#include "encoder.h"

#include "ejpeg.h"
#include "ebuffer.h"
#include "eparser.h"
#include "win_dct.h"
#include "huffman.h"
#include "fpxmem.h"

#define SHIFT 128
#define BOUND 1023
#define Q_PRECISION 50
#define MAX_Q 255

int izigzag_index[] =
{
  0,  1,  8, 16,  9,  2,  3, 10,
  17, 24, 32, 25, 18, 11,  4,  5,
  12, 19, 26, 33, 40, 48, 41, 34,
  27, 20, 13,  6,  7, 14, 21, 28,
  35, 42, 49, 56, 57, 50, 43, 36,
  29, 22, 15, 23, 30, 37, 44, 51,
  58, 59, 52, 45, 38, 31, 39, 46,
  53, 60, 61, 54, 47, 55, 62, 63};

static int csize[] = {
  0,
  1,
  2, 2,
  3, 3, 3, 3,
  4, 4, 4, 4, 4, 4, 4, 4,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8};


#define ENCODE_HUFFMAN(val, huffman) \
    EB_Write_Bits(huffman->ehufcode[val], huffman->ehufsize[val])

#ifdef DEBUG
/*
 * Programming Error (i.e. something wrong with the program).
 */
void debug(message)
char *message;
{
  fprintf(stderr,"**** SYSTEM ERROR: %s\n", message);
  exit(1);
};
#endif /* DEBUG */

void Scale_Char_Matrix(int numerator, int denominator, unsigned char *in, int *out)
{
  int i, val, inp;
  for (i = 0; i < 64; i++) {
    inp = (int) *in++;
    val = (inp * numerator) / denominator;
    if (val < 1) {
      *out++ = 1;
    } else if (val > 255) {
      *out++ = 255;
    } else {
      *out++ = val;
    }
  }
}

void Clear_Last_DC(JPEG_STRUCT *jpeg_struct)
{
  jpeg_struct->en_last_dc[0] = jpeg_struct->en_last_dc[1] = 
      jpeg_struct->en_last_dc[2] = jpeg_struct->en_last_dc[3] = 0;
}

void EN_Encode_DC(int dc_val, int comp, HUFFMAN_TABLE *huffman, 
JPEG_STRUCT *jpeg_struct)
{
  int diff, s, cofac;

  cofac = ((diff = dc_val - jpeg_struct->en_last_dc[comp]) < 0) ? -diff : diff;
  s = (cofac < 256) ? csize[cofac] : csize[cofac >> 8] + 8;
  jpeg_struct->en_last_dc[comp] = dc_val;

  /* Encode size */
  ENCODE_HUFFMAN(s, huffman);

  /* Encode difference */
  if (diff < 0)
    EB_Write_Bits(diff-1, s);
  else
    EB_Write_Bits(diff  , s);
}


void EN_Encode_Block(int *block,
int comp, /* image component number */
HUFFMAN_TABLE *dcHuffman,
HUFFMAN_TABLE *acHuffman,
int *quant,
JPEG_STRUCT *jpeg_struct)
{
  int r, data, ssss, *zig;
#define i comp

  Dct(block);

  /* Quantize and Encode DC component. No need to check Bound, since 
       data must be 11-bit signed integer, i.e. [-1023, 1023]        */

  data = SCALEM((long)block[0]*(*quant++));
  EN_Encode_DC(data, comp, dcHuffman, jpeg_struct);

  /* Quantize, Zigzag, then Encode AC coefficients. No Bound - see above */
  for (zig = izigzag_index + 1, r = 0, i = 64; --i > 0;) {
    if ((data = SCALEM((long)block[*zig++]*(*quant++))) == 0) {
      if (i == 1) {
        ENCODE_HUFFMAN(0, acHuffman);
        break;
      }
      r++;      /* Increment run-length of zeroes */
    } else if (data > 0) {
      while (r > 15) {      /* If run-length > 15 -> runlen extension */
        ENCODE_HUFFMAN(240, acHuffman);
        r -= 16;
      }
      if (data < 256) {     /* Find code byte */
        r = (r << 4) + (ssss = csize[data]);
      } else {
        r = (r << 4) + (ssss = csize[data >> 8] + 8);
      }
      ENCODE_HUFFMAN(r, acHuffman);   /* Encode RLE code */
      EB_Write_Bits (data, ssss);     /* Followed by sig. bits */
      r = 0;
    } else {
      while (r > 15) {      /* If run-length > 15 -> runlen extension */
        ENCODE_HUFFMAN(240, acHuffman);
        r -= 16;
      }
      if (data > -256) {              /* Find code byte */
        r = (r << 4) + (ssss = csize[-data]);
      } else {
        r = (r << 4) + (ssss = csize[(-data) >> 8] + 8);
      }
      ENCODE_HUFFMAN(r, acHuffman);   /* Encode RLE code */
      EB_Write_Bits (data-1, ssss);   /* Followed by sig. bits */
      r = 0;
    }
  }
#undef i
}


/*
 * Return 0 if successful, -1 if EOF
 */
int EN_Encode_Scan_Gray(unsigned char *buf, int width, int height, 
JPEG_STRUCT *jpeg_struct )
{
  /* Routine to create macroblocks for Grayscale images

       buf: points to the input data
       width and heigh are already in multiples of 8; i.e. data
       is already padded.
       Some error checking should be done at the higher level.
    */
  int j, i, k, nvblocks, nhblocks;
  int t, skip;
  /*  HANDLE lBlock; */
  int block[64];
  int *iptr;
  unsigned char *buf_ptr;

  Clear_Last_DC(jpeg_struct);

  nvblocks = height/8;
  nhblocks = width/8;
  skip = width -8;

  for (i=0; i < nhblocks; i++) {
    for(j=0; j < nhblocks; j++) {
      iptr = block;
      buf_ptr = buf + i*8*width + j*8;
      for(k = 8; k > 0; k--) {
        for(t = 8; t > 0; t--) {
          *iptr++ = *buf_ptr++ - SHIFT;
        }
        buf_ptr = buf_ptr + skip;
      }
      EN_Encode_Block(block,0, &jpeg_struct->huff[0],&jpeg_struct->huff[1],
          jpeg_struct->luminanceQuantization, jpeg_struct);
    } /* end on j */
  } /* end of vertical blocks (i) */
  return(0);
}


/*
 * Return 0 if successful, -1 if EOF
 */
int EN_Encode_Scan_Color11(unsigned char *buf,
int width,
int height,
int interleaved,
JPEG_STRUCT *jpeg_struct)
/* 
 * Routine to generate macroblocks for NON-subsampled data (1:1)
 * buf: a pointer to the data (2 color components)
 * if  interleaved=1, data is interleaved (C1C2 C1C2 ...)
 * else it is not (C1C1 ... C1C2C2 ... C2).
 *
 * width and heigh are already in multiples of 8; i.e. data
 * is already padded.
 * Some error checking should be done at the higher level.
*/
{
  unsigned char *buf_ptr;
  int *block1, *block2;
  int *p1, *p2, i, j, k, t, skip, block_row_skip;
  int nvMCU, nhMCU;         /* number of FULL MCU vert. and hori. */
  unsigned char *c1, *c2;

  Clear_Last_DC(jpeg_struct);

  nvMCU = (height)/8;
  nhMCU = (width)/8;

  block1 = block2 = NULL;         /* CHG_JPEG_MEM_FIX - used for allocation checking */
  block1 = (int *) FPX_malloc(64 * sizeof(int));
  block2 = (int *) FPX_malloc(64 * sizeof(int));

  if(interleaved == 1) {
    skip = (2*width) - 16;
    block_row_skip = 2*width*8;
    for (i = 0; i < nvMCU; i++) {
      for (j = 0; j < nhMCU; j++) {
        /* Construct 2 blocks within each MCU */

        p1 = block1; 
        p2 = block2;
        buf_ptr = buf + i*block_row_skip + j*16;
        for (k = 8; k > 0; k--) {
          for (t = 8; t > 0; t--) {
            *p1++ = *buf_ptr++ - SHIFT;
            *p2++ = *buf_ptr++ - SHIFT;
          }
          buf_ptr = buf_ptr + skip;
        }

        EN_Encode_Block(block1, 0, &jpeg_struct->huff[0], &jpeg_struct->huff[1], 
            jpeg_struct->luminanceQuantization,jpeg_struct);

        EN_Encode_Block(block2, 1, &jpeg_struct->huff[2], &jpeg_struct->huff[3], 
            jpeg_struct->chrominanceQuantization,jpeg_struct);
      } /* end of j */
    } /* end of i */
  } /* end  of interleaved data case */
  else {
    /************************/
    /* Non interleaved data */
    /************************/
    skip = width - 8;
    block_row_skip = width*8;
    for (i = 0; i < nvMCU; i++) {
      for (j = 0; j < nhMCU; j++) {
        /* Construct 2 blocks within each MCU */

        p1 = block1; 
        p2 = block2;
        c1 = buf + i*block_row_skip + j*8;
        c2 = c1 + width * height;
        for (k = 8; k > 0; k--) {
          for (t = 8; t > 0; t--) {
            *p1++ = *c1++ - SHIFT;
            *p2++ = *c2++ - SHIFT;
          }
          c1 = c1 + skip;
          c2 = c2 + skip;
        }
        EN_Encode_Block(block1, 0, &jpeg_struct->huff[0], &jpeg_struct->huff[1], 
            jpeg_struct->luminanceQuantization,jpeg_struct);
        EN_Encode_Block(block2, 1, &jpeg_struct->huff[2], &jpeg_struct->huff[3], 
            jpeg_struct->chrominanceQuantization,jpeg_struct);
      } /* end of j */
    } /* end of i */
  } /* end of else loop */

  FPX_free(block1);
  FPX_free(block2);

  return(0);
}


/*
 * Return 0 if successful, -1 if EOF
 */
int EN_Encode_Scan_Color111(unsigned char *buf,
int width,
int height,
int interleaved, 
JPEG_STRUCT *jpeg_struct)
/* 
 * Routine to generate macroblocks for NON-subsampled data (1:1:1)
 * buf: a pointer to the data (3 color components)
 * if  interleaved=1, data is interleaved (RGB RGB ..)
 * else it is not.
 *
 * width and heigh are already in multiples of 8; i.e. data
 * is already padded.
 * Some error checking should be done at the higher level.
*/
{
  unsigned char *buf_ptr;
  int *block1, *block2, *block3;
  int *p1, *p2, *p3, i, j, k, t, skip, block_row_skip;
  int nvMCU, nhMCU;         /* number of FULL MCU vert. and hori. */
  unsigned char *c1, *c2, *c3;

  block1 = block2 = block3 = NULL;  /* CHG_JPEG_MEM_FIX - used for allocation checking */
  block1 = (int *) FPX_malloc(64 * sizeof(int));
  block2 = (int *) FPX_malloc(64 * sizeof(int));
  block3 = (int *) FPX_malloc(64 * sizeof(int));
  
  /* CHG_JPEG_MEM_FIX - added the following allocation check */
  if ((block1 == 0) || (block2 == 0) || (block3 == 0)) {
    if (block1)   FPX_free(block1);
    if (block2)   FPX_free(block2);
    return (EJPEG_ERROR_MEM);
  }

  Clear_Last_DC(jpeg_struct);

  nvMCU = (height)/8;
  nhMCU = (width)/8;

  if(interleaved == 1) {
    skip = (3*width) - 24;
    block_row_skip = 3*width*8;
    for (i = 0; i < nvMCU; i++) {
      for (j = 0; j < nhMCU; j++) {
        /* Construct 3 blocks within each MCU */

        p1 = block1; 
        p2 = block2; 
        p3 = block3;
        buf_ptr = buf + i*block_row_skip + j*24;
        for (k = 8; k > 0; k--) {
          for (t = 8; t > 0; t--) {
            *p1++ = *buf_ptr++ - SHIFT;
            *p2++ = *buf_ptr++ - SHIFT;
            *p3++ = *buf_ptr++ - SHIFT;
          }
          buf_ptr = buf_ptr + skip;
        }
        EN_Encode_Block(block1, 0, &jpeg_struct->huff[0], &jpeg_struct->huff[1], 
            jpeg_struct->luminanceQuantization,jpeg_struct);
        EN_Encode_Block(block2, 1, &jpeg_struct->huff[2], &jpeg_struct->huff[3], 
            jpeg_struct->chrominanceQuantization,jpeg_struct);
        EN_Encode_Block(block3, 2, &jpeg_struct->huff[4], &jpeg_struct->huff[5], 
            jpeg_struct->tbl3_Quantization,jpeg_struct);
      } /* end of j */
    } /* end of i */
  } /* end  of interleaved data case */
  else {
    /************************/
    /* Non interleaved data */
    /************************/
    skip = width - 8;
    block_row_skip = width*8;
    for (i = 0; i < nvMCU; i++) {
      for (j = 0; j < nhMCU; j++) {
        /* Construct 3 blocks within each MCU */

        p1 = block1; 
        p2 = block2; 
        p3 = block3;
        c1 = buf + i*block_row_skip + j*8;
        c2 = c1 + width * height;
        c3 = c2 + width * height;
        for (k = 8; k > 0; k--) {
          for (t = 8; t > 0; t--) {
            *p1++ = *c1++ - SHIFT;
            *p2++ = *c2++ - SHIFT;
            *p3++ = *c3++ - SHIFT;
          }
          c1 = c1 + skip;
          c2 = c2 + skip;
          c3 = c3 + skip;
        }
        EN_Encode_Block(block1, 0, &jpeg_struct->huff[0], &jpeg_struct->huff[1], 
            jpeg_struct->luminanceQuantization,jpeg_struct);
        EN_Encode_Block(block2, 1, &jpeg_struct->huff[2], &jpeg_struct->huff[3], 
            jpeg_struct->chrominanceQuantization,jpeg_struct);
        EN_Encode_Block(block3, 2, &jpeg_struct->huff[4], &jpeg_struct->huff[5], 
            jpeg_struct->tbl3_Quantization,jpeg_struct);
      } /* end of j */
    } /* end of i */
  } /* end of else loop */

  FPX_free(block1);
  FPX_free(block2);
  FPX_free(block3);

  return(0);
}


/*
 * Return 0 if successful, -1 if EOF
 */
int EN_Encode_Scan_Color1111(unsigned char *buf,
int width, int height,
int interleaved, 
JPEG_STRUCT *jpeg_struct)
/* 
 * Routine to generate macroblocks for NON-subsampled data (1:1:1:1)
 * buf: a pointer to the data (4 color components)
 * if  interleaved=1, data is interleaved (c1c2c3c4 c1c2c3c4 ...)
 * else it is not (c1c1...c1c2c2...c2c3c3...c3c4c4...c4).
 *
 * width and heigh are already in multiples of 8; i.e. data
 * is already padded.
 * Some error checking should be done at the higher level.
*/
{
  unsigned char *buf_ptr;
  int *block1, *block2, *block3, *block4;
  int *p1, *p2, *p3, *p4, i, j, k, t, skip, block_row_skip;
  int nvMCU, nhMCU;         /* number of FULL MCU vert. and hori. */
  unsigned char *c1, *c2, *c3, *c4;

  block1 = block2 = block3 = block4 = NULL; /* CHG_JPEG_MEM_FIX - for allocation checking */
  block1 = (int *) FPX_malloc(64 * sizeof(int));
  block2 = (int *) FPX_malloc(64 * sizeof(int));
  block3 = (int *) FPX_malloc(64 * sizeof(int));
  block4 = (int *) FPX_malloc(64 * sizeof(int));
  
  /* CHG_JPEG_MEM_FIX - added the following allocation check */
  if ((block1 == 0) || (block2 == 0) || (block3 == 0) || (block4 == 0)) {
    if (block1)   FPX_free(block1);
    if (block2)   FPX_free(block2);
    if (block3)   FPX_free(block3);
    return (EJPEG_ERROR_MEM);
  }

  Clear_Last_DC(jpeg_struct);

  nvMCU = (height)/8;
  nhMCU = (width)/8;

  if(interleaved == 1) {
    skip = (4*width) - 32;
    block_row_skip = 4*width*8;
    for (i = 0; i < nvMCU; i++) {
      for (j = 0; j < nhMCU; j++) {
        /* Construct 4 blocks within each MCU */

        p1 = block1; 
        p2 = block2; 
        p3 = block3; 
        p4 = block4;
        buf_ptr = buf + i*block_row_skip + j*32;

        /* Should this run from seven to zero?  Will this cause a whammy?  KAW */
        for (k = 8; k > 0; k--) {
          for (t = 8; t > 0; t--) {
            *p1++ = *buf_ptr++ - SHIFT;
            *p2++ = *buf_ptr++ - SHIFT;
            *p3++ = *buf_ptr++ - SHIFT;
            *p4++ = *buf_ptr++ - SHIFT;
          }
          buf_ptr = buf_ptr + skip;
        }
        EN_Encode_Block(block1, 0, &jpeg_struct->huff[0], &jpeg_struct->huff[1], 
            jpeg_struct->luminanceQuantization,jpeg_struct);
        EN_Encode_Block(block2, 1, &jpeg_struct->huff[2], &jpeg_struct->huff[3], 
            jpeg_struct->chrominanceQuantization,jpeg_struct);
        EN_Encode_Block(block3, 2, &jpeg_struct->huff[4], &jpeg_struct->huff[5], 
            jpeg_struct->tbl3_Quantization,jpeg_struct);
        EN_Encode_Block(block4, 3, &jpeg_struct->huff[6], &jpeg_struct->huff[7], 
            jpeg_struct->tbl4_Quantization,jpeg_struct);
      } /* end of j */
    } /* end of i */
  } /* end  of interleaved data case */
  else {
    /************************/
    /* Non interleaved data */
    /************************/
    skip = width - 8;
    block_row_skip = width*8;
    for (i = 0; i < nvMCU; i++) {
      for (j = 0; j < nhMCU; j++) {
        /* Construct 4 blocks within each MCU */

        p1 = block1; 
        p2 = block2; 
        p3 = block3; 
        p4 = block4;
        c1 = buf + i*block_row_skip + j*8;
        c2 = c1 + width * height;
        c3 = c2 + width * height;
        c4 = c3 + width * height;
        for (k = 8; k > 0; k--) {
          for (t = 8; t > 0; t--) {
            *p1++ = *c1++ - SHIFT;
            *p2++ = *c2++ - SHIFT;
            *p3++ = *c3++ - SHIFT;
            *p4++ = *c4++ - SHIFT;
          }
          c1 = c1 + skip;
          c2 = c2 + skip;
          c3 = c3 + skip;
          c4 = c4 + skip;
        }
        EN_Encode_Block(block1, 0, &jpeg_struct->huff[0], &jpeg_struct->huff[1], 
            jpeg_struct->luminanceQuantization,jpeg_struct);
        EN_Encode_Block(block2, 1, &jpeg_struct->huff[2], &jpeg_struct->huff[3], 
            jpeg_struct->chrominanceQuantization,jpeg_struct);
        EN_Encode_Block(block3, 2, &jpeg_struct->huff[4], &jpeg_struct->huff[5], 
            jpeg_struct->tbl3_Quantization,jpeg_struct);
        EN_Encode_Block(block4, 3, &jpeg_struct->huff[6], &jpeg_struct->huff[7], 
            jpeg_struct->tbl4_Quantization,jpeg_struct);
      } /* end of j */
    } /* end of i */
  } /* end of else loop */

  FPX_free(block1);
  FPX_free(block2);
  FPX_free(block3);
  FPX_free(block4);
  return(0);
}


/*
 * Return 0 if successful, -1 if EOF
 */
int EN_Encode_Scan_Color4114(unsigned char *buf,
int width, int height,
int interleaved,
JPEG_STRUCT *jpeg_struct)
{
  /* 
 * Routine to generate macroblocks for subsampled data in 4:1:1:4 format
 * i.e., 4-channel data with the two chroma channels subsampled by 2x 
 * in both dimensions. An example of such an image is a YCrCb-A image
 * with the Cr-Cb channels subsampled by 2x.
 *
 * <buf>: a pointer to the data (4 color components)
 * if  interleaved=1, data is interleaved 
 * (c1c1c1c1 c2 c3 c4c4c4c4, c1c1c1c1 c2 c3 c4c4c4c4, ...)
 * else it is not ( data is in scan-line by scan-line format).
 *
 * width and heigh are already in multiples of 16; i.e. data
 * is already padded and subsampled as necessary.
 *
 * Some error checking should be done at the higher level
 * for the width height dimensions.
*/
  unsigned char *buf_ptr, *c1_ptr, *c2_ptr, *c3_ptr;
  int *block1, *block2, *block3, *block4;
  int *p1, *p2, *p3, *p4, i, j, k, l, t, skip, skip2,skip_row_data;
  int *p1_2, *p2_2;
  int nvMCU, nhMCU;  /* number of FULL MCU vert. and hor. */
  int *block5, *block6;
  int *block7, *block8, *block9, *block10;
  int *p5, *p6, *p5_2, *p6_2;
  unsigned char *c4_ptr;

  block1 = block2 = block3 = block4 = NULL;   /* CHG_JPEG_MEM_FIX - for allocation checking */
  block5 = block6 = NULL;             
  block7 = block8 = block9 = block10 = NULL;  

  block1 = (int *) FPX_malloc(64 * sizeof(int)); /* Luma block */
  block2 = (int *) FPX_malloc(64 * sizeof(int)); /* Luma block */
  block3 = (int *) FPX_malloc(64 * sizeof(int)); /* Luma block */
  block4 = (int *) FPX_malloc(64 * sizeof(int)); /* Luma block */

  if ((block1 == 0) || (block2 == 0) || (block3 == 0) || (block4 == 0))
    goto Exit;                  /* CHG_JPEG_MEM_FIX - Added allocation test */

  block5 = (int *) FPX_malloc(64 * sizeof(int)); /* 1rst-chroma block */
  block6 = (int *) FPX_malloc(64 * sizeof(int)); /* 2nd-chroma block  */

  if ((block5 == 0) || (block6 == 0))
    goto Exit;                  /* CHG_JPEG_MEM_FIX - Added allocation test */

  block7  = (int *) FPX_malloc(64 * sizeof(int)); /* Alpha channel */
  block8  = (int *) FPX_malloc(64 * sizeof(int)); /* Alpha channel */
  block9  = (int *) FPX_malloc(64 * sizeof(int)); /* Alpha channel */
  block10 = (int *) FPX_malloc(64 * sizeof(int)); /* Alpha channel */

  if ((block7 == 0) || (block8 == 0) || (block9 == 0) || (block10 == 0))
    goto Exit;                  /* CHG_JPEG_MEM_FIX - Added allocation test */

  Clear_Last_DC(jpeg_struct);

  nvMCU = height/16;
  nhMCU = width/16;

  /* Interleaved Data */
  if (interleaved == 1) {
    /* if interleaved,  a line has (width/2 * 10) pixels */
    skip = width*5 - 80; /* 80 = 8 subsampled blocks * 10 bytes/block  */
    skip_row_data = 8*5*width;  /* One 16x16 block-row bytes */
    for (i=0; i < nvMCU; i++) {
      for (j=0; j < nhMCU; j++) {
        buf_ptr = buf + i*skip_row_data + j*80;

        p3 = block5; 
        p4 = block6 ;    /* chroma blocks */
        for (l=2; l>0; l--) { /* Top/Bottom row of Luma-blocks */
          if (l==2) {
            p1 = block1; 
            p2 = block2; /* left-right luma blocks, odd rows */
            p1_2 = p1+8; 
            p2_2 = p2+8; /* even rows */
            p5 = block7; 
            p6 = block8; /* left-right luma blocks, odd rows */
            p5_2 = p5+8; 
            p6_2 = p6+8; /* even rows */
          } else {
            p1 = block3; 
            p2 = block4; /* left-right luma blocks, odd rows */
            p1_2 = p1+8; 
            p2_2 = p2+8; /* even rows */
            p5 = block9; 
            p6 = block10; /* left-right luma blocks, odd rows */
            p5_2 = p5+8; 
            p6_2 = p6+8; /* even rows */
          }

          for(k=4; k > 0; k--) { /* vertical subsampled grid */
            for(t=4; t > 0; t--) {  /* hor. subsampled grid Left-Block */
              *p1++ = *buf_ptr++ - SHIFT;
              *p1++ = *buf_ptr++ - SHIFT;
              *p1_2++ = *buf_ptr++ - SHIFT;
              *p1_2++ = *buf_ptr++ - SHIFT;
              *p3++ = *buf_ptr++ - SHIFT;
              *p4++ = *buf_ptr++ - SHIFT;
              *p5++ = *buf_ptr++ - SHIFT;
              *p5++ = *buf_ptr++ - SHIFT;
              *p5_2++ = *buf_ptr++ - SHIFT;
              *p5_2++ = *buf_ptr++ - SHIFT;
            } /* t-loop */
            for(t=4; t > 0; t--) { /* hor. subsampled grid Right-Block */
              *p2++ = *buf_ptr++ - SHIFT;
              *p2++ = *buf_ptr++ - SHIFT;
              *p2_2++ = *buf_ptr++ - SHIFT;
              *p2_2++ = *buf_ptr++ - SHIFT;
              *p3++ = *buf_ptr++ - SHIFT;
              *p4++ = *buf_ptr++ - SHIFT;
              *p6++ = *buf_ptr++ - SHIFT;
              *p6++ = *buf_ptr++ - SHIFT;
              *p6_2++ = *buf_ptr++ - SHIFT;
              *p6_2++ = *buf_ptr++ - SHIFT;
            } /* t-loop */
            p1 = p1 + 8; 
            p2 = p2 +8;
            p1_2 = p1_2 + 8; 
            p2_2 = p2_2 +8;
            p5 = p5 + 8; 
            p6 = p6 +8;
            p5_2 = p5_2 + 8; 
            p6_2 = p6_2 +8;
            buf_ptr = buf_ptr + skip;
          } /* k-loop */
        }  /* end on l */

        EN_Encode_Block(block1, 0, &jpeg_struct->huff[0], &jpeg_struct->huff[1], 
            jpeg_struct->luminanceQuantization,jpeg_struct);
        EN_Encode_Block(block2, 0, &jpeg_struct->huff[0], &jpeg_struct->huff[1], 
            jpeg_struct->luminanceQuantization,jpeg_struct);
        EN_Encode_Block(block3, 0, &jpeg_struct->huff[0], &jpeg_struct->huff[1], 
            jpeg_struct->luminanceQuantization,jpeg_struct);
        EN_Encode_Block(block4, 0, &jpeg_struct->huff[0], &jpeg_struct->huff[1], 
            jpeg_struct->luminanceQuantization,jpeg_struct);

        EN_Encode_Block(block5, 1, &jpeg_struct->huff[2], &jpeg_struct->huff[3], 
            jpeg_struct->chrominanceQuantization,jpeg_struct);
        EN_Encode_Block(block6, 2, &jpeg_struct->huff[4], &jpeg_struct->huff[5], 
            jpeg_struct->tbl3_Quantization,jpeg_struct);

        EN_Encode_Block(block7, 3, &jpeg_struct->huff[6], &jpeg_struct->huff[7], 
            jpeg_struct->tbl4_Quantization,jpeg_struct);
        EN_Encode_Block(block8, 3, &jpeg_struct->huff[6], &jpeg_struct->huff[7], 
            jpeg_struct->tbl4_Quantization,jpeg_struct);
        EN_Encode_Block(block9, 3, &jpeg_struct->huff[6], &jpeg_struct->huff[7], 
            jpeg_struct->tbl4_Quantization,jpeg_struct);
        EN_Encode_Block(block10, 3, &jpeg_struct->huff[6], &jpeg_struct->huff[7], 
            jpeg_struct->tbl4_Quantization,jpeg_struct);

      } /* end on j */
    } /* end on i */
  } /* end of interleaved */
  /************************/
  /* Non-Interleaved Data */
  /************************/
  else {
    skip = width - 16;
    skip2 = (width/2) - 8;
    for(i = 0; i < nvMCU ; i++) {
      for(j = 0; j < nhMCU ; j++) {

        p3 = block5; 
        p4 = block6 ;    /* chroma blocks */
        c1_ptr = buf + j*16;
        c2_ptr = buf + width*height + j*8;
        c3_ptr = c2_ptr + (width*height)/4;
        c4_ptr = c3_ptr + (width*height)/4;

        for (l=2; l>0; l--) {
          if (l==2) {
            p1 = block1; 
            p2 = block2;  /* left-right luma blocks */
            p5 = block7; 
            p6 = block8;  /* left-right alpha blocks */
          } else {
            p1 = block3; 
            p2 = block4;  /* left-right luma blocks */
            p5 = block9; 
            p6 = block10;  /* left-right alpha blocks */
          }
          for(k=8; k > 0; k--) {
            /* Top */
            for(t=8; t > 0; t--) {
              *p1++ = *c1_ptr++ - SHIFT;
              *p5++ = *c4_ptr++ - SHIFT;
            } /* t-loop */
            /* bottom */
            for(t=8; t > 0; t--) {
              *p2++ = *c1_ptr++ - SHIFT;
              *p6++ = *c4_ptr++ - SHIFT;
            } /* t-loop */
            /* buf_ptr = buf_ptr + skip; */
          } /* k-loop */
        }  /* end on l */
        /* Fill-up the chroma 8x8 blocks */

        for(k=8; k> 0; k--){
          for(t=8; t > 0; t--) {
            *p3++ = *c2_ptr++ - SHIFT;
            *p4++ = *c3_ptr++ - SHIFT;
          }
          c2_ptr = c2_ptr + skip2;
          c3_ptr = c3_ptr + skip2;
        } /* end on k*/

        EN_Encode_Block(block1, 0, &jpeg_struct->huff[0], &jpeg_struct->huff[1], 
            jpeg_struct->luminanceQuantization,jpeg_struct);
        EN_Encode_Block(block2, 0, &jpeg_struct->huff[0], &jpeg_struct->huff[1], 
            jpeg_struct->luminanceQuantization,jpeg_struct);
        EN_Encode_Block(block3, 0, &jpeg_struct->huff[0], &jpeg_struct->huff[1], 
            jpeg_struct->luminanceQuantization,jpeg_struct);
        EN_Encode_Block(block4, 0, &jpeg_struct->huff[0], &jpeg_struct->huff[1], 
            jpeg_struct->luminanceQuantization,jpeg_struct);

        EN_Encode_Block(block5, 1, &jpeg_struct->huff[2], &jpeg_struct->huff[3], 
            jpeg_struct->chrominanceQuantization,jpeg_struct);
        EN_Encode_Block(block6, 2, &jpeg_struct->huff[4], &jpeg_struct->huff[5], 
            jpeg_struct->tbl3_Quantization,jpeg_struct);

        EN_Encode_Block(block7, 3, &jpeg_struct->huff[6], &jpeg_struct->huff[7], 
            jpeg_struct->tbl4_Quantization,jpeg_struct);
        EN_Encode_Block(block8, 3, &jpeg_struct->huff[6], &jpeg_struct->huff[7], 
            jpeg_struct->tbl4_Quantization,jpeg_struct);
        EN_Encode_Block(block9, 3, &jpeg_struct->huff[6], &jpeg_struct->huff[7], 
            jpeg_struct->tbl4_Quantization,jpeg_struct);
        EN_Encode_Block(block10, 3, &jpeg_struct->huff[6], &jpeg_struct->huff[7], 
            jpeg_struct->tbl4_Quantization,jpeg_struct);

      } /* end on j */
    } /* end on i */
  } /* end of non-interleaved data. */

  Exit:
  FPX_free(block1);
  FPX_free(block2);
  FPX_free(block3);
  FPX_free(block4);
  FPX_free(block5);
  FPX_free(block6);
  FPX_free(block7);
  FPX_free(block8);
  FPX_free(block9);
  FPX_free(block10);

  return(0);
}


int EN_Encode_Scan_Color422(unsigned char *buf,
int width, int height,
int interleaved,
JPEG_STRUCT *jpeg_struct)
{
  /* 
 * Routine to generate macroblocks for subsampled data in 4:2:2
 *
 * <buf>: a pointer to the data (3 color components)
 * if  interleaved=1, data is interleaved (c1c1 c2 c3, c1c1 c2 c3)
 * else it is not( data is in scan-line by scan -line format).
 *
 * width and height are already in multiples of 8/16; i.e. data
 * is already padded and subsampled as necessary.
 *
 * Some error checking should be done at the higher level
 * for the width height dimensions.
*/
  unsigned char *buf_ptr;
  unsigned char *c1_ptr, *c2_ptr, *c3_ptr;
  int *block1, *block2, *block3, *block4;
  int *p1, *p2, *p3, *p4, i, j, k, t;
  int skip,skip2,skip_row_data, skip_row_data1,skip_row_data2;
  int nvMCU, nhMCU;  /* number of FULL MCU vert. and hor. */

  block1 = block2 = block3 = block4 = NULL; /* CHG_JPEG_MEM_FIX -  for allocation checking */
  block1 = (int *) FPX_malloc(64 * sizeof(int));
  block2 = (int *) FPX_malloc(64 * sizeof(int));
  block3 = (int *) FPX_malloc(64 * sizeof(int));
  block4 = (int *) FPX_malloc(64 * sizeof(int));
  
  /* CHG_JPEG_MEM_FIX - added the following allocation check */
  if ((block1 == 0) || (block2 == 0) || (block3 == 0) || (block4 == 0)) {
    if (block1)   FPX_free(block1);
    if (block2)   FPX_free(block2);
    if (block3)   FPX_free(block3);
    return (EJPEG_ERROR_MEM);
  }

  Clear_Last_DC(jpeg_struct);

  nhMCU = width/16;
  nvMCU = height/8;

  /* Interleaved Data */
  if (interleaved == 1) {
    /* if interleaved, a line has (width/2 * 4) pixels (1byte/pixel) */
    /* A subsampled symbol has 4 bytes. */
    skip = width*2 - 32;          /* 32 = 8 subsampled symbols x 4bytes/ss-symbol */
    skip_row_data = 8*4*width/2; /* One 8x16 block-row bytes */
    for (i=0; i < nvMCU; i++) {
      for (j=0; j < nhMCU; j++) {
        p3 = block3; 
        p4 = block4 ;    /* chroma blocks */
        buf_ptr = buf + i*skip_row_data + j*32;

        p1 = block1; 
        p2 = block2;  /* left-right blocks */
        for(k=8; k > 0; k--) {
          for(t=4; t > 0; t--) {
            *p1++ = *buf_ptr++ - SHIFT;
            *p1++ = *buf_ptr++ - SHIFT;
            *p3++ = *buf_ptr++ - SHIFT;
            *p4++ = *buf_ptr++ - SHIFT;
          } /* t-loop */
          for(t=4; t > 0; t--) {
            *p2++ = *buf_ptr++ - SHIFT;
            *p2++ = *buf_ptr++ - SHIFT;
            *p3++ = *buf_ptr++ - SHIFT;
            *p4++ = *buf_ptr++ - SHIFT;
          } /* t-loop */
          buf_ptr = buf_ptr + skip;
        } /* k-loop */
        EN_Encode_Block(block1, 0, &jpeg_struct->huff[0], &jpeg_struct->huff[1], 
            jpeg_struct->luminanceQuantization,jpeg_struct);
        EN_Encode_Block(block2, 0, &jpeg_struct->huff[0], &jpeg_struct->huff[1], 
            jpeg_struct->luminanceQuantization,jpeg_struct);
        EN_Encode_Block(block3, 1, &jpeg_struct->huff[2], &jpeg_struct->huff[3], 
            jpeg_struct->chrominanceQuantization,jpeg_struct);
        EN_Encode_Block(block4, 2, &jpeg_struct->huff[4], &jpeg_struct->huff[5], 
            jpeg_struct->tbl3_Quantization,jpeg_struct);

      } /* end on j */
    } /* end on i */
  } /* end of interleaved */
  /************************/
  /* Non-Interleaved Data */
  /************************/
  else {
    skip = width - 16;
    skip2 = (width/2) - 8;
    skip_row_data1 = 8*width;  /* a row of luma-blocks */
    skip_row_data2 = 8*width/2; /* a row of chroma-blocks */
    for(i = 0; i < nvMCU ; i++) {
      for(j = 0; j < nhMCU ; j++) {
        p3 = block3; 
        p4 = block4 ;    /* chroma blocks */
        c1_ptr = buf + i*skip_row_data1 + j*16;
        c2_ptr = buf + width*height + i*skip_row_data2 + j*8;
        c3_ptr = c2_ptr + (width*height)/4;

        p1 = block1; 
        p2 = block2;  /* left-right blocks */
        for(k=8; k > 0; k--) {
          for(t=8; t > 0; t--) { /* left block */
            *p1++ = *c1_ptr++ - SHIFT;
          } /* t-loop */

          for(t=8; t > 0; t--) { /* tight block */
            *p2++ = *c1_ptr++ - SHIFT;
          } /* t-loop */
          c1_ptr += skip;
        } /* k-loop */
        EN_Encode_Block(block1, 0, &jpeg_struct->huff[0], &jpeg_struct->huff[1], 
            jpeg_struct->luminanceQuantization,jpeg_struct);
        EN_Encode_Block(block2, 0, &jpeg_struct->huff[0], &jpeg_struct->huff[1], 
            jpeg_struct->luminanceQuantization,jpeg_struct);

        /* Fill-up the chroma 8x8 blocks */
        for(k=8; k> 0; k--){
          for(t=8; t > 0; t--) {
            *p3++ = *c2_ptr++ - SHIFT;
            *p4++ = *c3_ptr++ - SHIFT;
          }
          c2_ptr = c2_ptr + skip2;
          c3_ptr = c3_ptr + skip2;
        } /* end on k*/
        EN_Encode_Block(block3, 1, &jpeg_struct->huff[2], &jpeg_struct->huff[3], 
            jpeg_struct->chrominanceQuantization,jpeg_struct);
        EN_Encode_Block(block4, 2, &jpeg_struct->huff[4], &jpeg_struct->huff[5], 
            jpeg_struct->tbl3_Quantization,jpeg_struct);
      } /* end on j */
    } /* end on i */
  } /* end of non-interleaved data. */
  FPX_free(block1);
  FPX_free(block2);
  FPX_free(block3);
  FPX_free(block4);

  return(0);
}


int EN_Encode_Scan_Color4224(unsigned char *buf,
int width, int height,
int interleaved,
JPEG_STRUCT *jpeg_struct)
{
  /* 
 * Routine to generate macroblocks for subsampled data in 4:2:2:4 format
 * 4-channel data, the middle 2 channels subsampled by 2x in hor. dimension only
 *
 * <buf>: a pointer to the data (4 color components)
 * if  interleaved=1, data is interleaved (c1c1 c2 c3 c4c4, c1c1 c2 c3 c4c4)
 * else it is not( data is in scan-line by scan -line format).
 *
 * width and height are already in multiples of 8/16; i.e. data
 * is already padded and subsampled as necessary.
 *
 * Some error checking should be done at the higher level
 * for the width height dimensions.
*/
  unsigned char *buf_ptr;
  unsigned char *c1_ptr, *c2_ptr, *c3_ptr, *c4_ptr;
  int *block1, *block2, *block3, *block4, *block5, *block6;
  int *p1, *p2, *p3, *p4, *p5, *p6, i, j, k, t;
  int skip,skip2,skip_row_data, skip_row_data1,skip_row_data2;
  int nvMCU, nhMCU;  /* number of FULL MCU vert. and hor. */

  block1 = block2 = block3 = block4= block5 = block6 = NULL;  /* CHG_JPEG_MEM_FIX - for allocation checking */
  block1 = (int *) FPX_malloc(64 * sizeof(int));
  block2 = (int *) FPX_malloc(64 * sizeof(int));
  block3 = (int *) FPX_malloc(64 * sizeof(int));
  block4 = (int *) FPX_malloc(64 * sizeof(int));
  block5 = (int *) FPX_malloc(64 * sizeof(int));
  block6 = (int *) FPX_malloc(64 * sizeof(int));

  if ((block1 == 0) || (block2 == 0) || (block3 == 0) || 
    (block4 == 0) || (block5 == 0) || (block6 == 0))
    goto Exit;                    /* CHG_JPEG_MEM_FIX - Added allocation test */

  Clear_Last_DC(jpeg_struct);

  nhMCU = width/16;
  nvMCU = height/8;

  /* Interleaved Data */
  if (interleaved == 1) {
    /* if interleaved, a line has (width/2 * 6) pixels (1byte/pixel) */
    /* A subsampled symbol has 6 bytes. */
    skip = width*3 - 48;          /* 48 = 8 subsampled symbols x 6bytes/ss-symbol */
    skip_row_data = 24*width; /* One 8x16 block-row bytes */
    /* 8 lines * (6*width/2)bytes/line */
    for (i=0; i < nvMCU; i++) {
      for (j=0; j < nhMCU; j++) {
        p3 = block3; 
        p4 = block4 ;    /* chroma blocks */
        buf_ptr = buf + i*skip_row_data + j*48;

        p1 = block1; 
        p2 = block2;  /* left-right luma blocks */
        p5 = block5; 
        p6 = block6;  /* left-right alpha blocks */
        for(k=8; k > 0; k--) {
          for(t=4; t > 0; t--) {
            *p1++ = *buf_ptr++ - SHIFT;
            *p1++ = *buf_ptr++ - SHIFT;
            *p3++ = *buf_ptr++ - SHIFT;
            *p4++ = *buf_ptr++ - SHIFT;
            *p5++ = *buf_ptr++ - SHIFT;
            *p5++ = *buf_ptr++ - SHIFT;
          } /* t-loop */
          for(t=4; t > 0; t--) {
            *p2++ = *buf_ptr++ - SHIFT;
            *p2++ = *buf_ptr++ - SHIFT;
            *p3++ = *buf_ptr++ - SHIFT;
            *p4++ = *buf_ptr++ - SHIFT;
            *p6++ = *buf_ptr++ - SHIFT;
            *p6++ = *buf_ptr++ - SHIFT;
          } /* t-loop */
          buf_ptr = buf_ptr + skip;
        } /* k-loop */
        EN_Encode_Block(block1, 0, &jpeg_struct->huff[0], &jpeg_struct->huff[1], 
            jpeg_struct->luminanceQuantization,jpeg_struct);
        EN_Encode_Block(block2, 0, &jpeg_struct->huff[0], &jpeg_struct->huff[1], 
            jpeg_struct->luminanceQuantization,jpeg_struct);
        EN_Encode_Block(block3, 1, &jpeg_struct->huff[2], &jpeg_struct->huff[3], 
            jpeg_struct->chrominanceQuantization,jpeg_struct);
        EN_Encode_Block(block4, 2, &jpeg_struct->huff[4], &jpeg_struct->huff[5], 
            jpeg_struct->tbl3_Quantization,jpeg_struct);
        EN_Encode_Block(block5, 3, &jpeg_struct->huff[6], &jpeg_struct->huff[7], 
            jpeg_struct->tbl4_Quantization,jpeg_struct);
        EN_Encode_Block(block6, 3, &jpeg_struct->huff[6], &jpeg_struct->huff[7], 
            jpeg_struct->tbl4_Quantization,jpeg_struct);

      } /* end on j */
    } /* end on i */
  } /* end of interleaved */
  /************************/
  /* Non-Interleaved Data */
  /************************/
  else {
    skip = width - 16;
    skip2 = (width/2) - 8;
    skip_row_data1 = 8*width;  /* a row of luma/alpha -blocks */
    skip_row_data2 = 8*width/2; /* a row of chroma-blocks */
    for(i = 0; i < nvMCU ; i++) {
      for(j = 0; j < nhMCU ; j++) {
        p3 = block3; 
        p4 = block4 ;    /* chroma blocks */
        c1_ptr = buf + i*skip_row_data1 + j*16;
        c2_ptr = buf + width*height + i*skip_row_data2 + j*8;
        c3_ptr = c2_ptr + (width*height)/4;
        c4_ptr = buf + 3*width*height/2 + i*skip_row_data1 + j*16;

        p1 = block1; 
        p2 = block2;  /* left-right luma blocks */
        p5 = block5; 
        p6 = block6;  /* left-right alpha blocks */
        for(k=8; k > 0; k--) {
          for(t=8; t > 0; t--) { /* left block */
            *p1++ = *c1_ptr++ - SHIFT;
            *p5++ = *c4_ptr++ - SHIFT;
          } /* t-loop */

          for(t=8; t > 0; t--) { /* tight block */
            *p2++ = *c1_ptr++ - SHIFT;
            *p6++ = *c4_ptr++ - SHIFT;
          } /* t-loop */
          c1_ptr += skip;
          c4_ptr += skip;
        } /* k-loop */
        EN_Encode_Block(block1, 0, &jpeg_struct->huff[0], &jpeg_struct->huff[1], 
            jpeg_struct->luminanceQuantization,jpeg_struct);
        EN_Encode_Block(block2, 0, &jpeg_struct->huff[0], &jpeg_struct->huff[1], 
            jpeg_struct->luminanceQuantization,jpeg_struct);

        /* Fill-up the chroma 8x8 blocks */
        for(k=8; k> 0; k--){
          for(t=8; t > 0; t--) {
            *p3++ = *c2_ptr++ - SHIFT;
            *p4++ = *c3_ptr++ - SHIFT;
          }
          c2_ptr = c2_ptr + skip2;
          c3_ptr = c3_ptr + skip2;
        } /* end on k*/
        EN_Encode_Block(block3, 1, &jpeg_struct->huff[2], &jpeg_struct->huff[3], 
            jpeg_struct->chrominanceQuantization,jpeg_struct);
        EN_Encode_Block(block4, 2, &jpeg_struct->huff[4], &jpeg_struct->huff[5], 
            jpeg_struct->tbl3_Quantization,jpeg_struct);

        /* Encode Alpha Blocks */
        EN_Encode_Block(block5, 3, &jpeg_struct->huff[6], &jpeg_struct->huff[7], 
            jpeg_struct->tbl4_Quantization,jpeg_struct);
        EN_Encode_Block(block6, 3, &jpeg_struct->huff[6], &jpeg_struct->huff[7], 
            jpeg_struct->tbl4_Quantization,jpeg_struct);

      } /* end on j */
    } /* end on i */
  } /* end of non-interleaved data. */
  Exit:                 /* CHG_JPEG_MEM_FIX - branch to here if mem alloc fails */
  FPX_free(block1);
  FPX_free(block2);
  FPX_free(block3);
  FPX_free(block4);
  FPX_free(block5);
  FPX_free(block6);

  return(0);
}


int EN_Encode_Scan_Color411(unsigned char *buf,
int width, int height,
int interleaved,
JPEG_STRUCT *jpeg_struct)
{
  /* 
 * Routine to generate macroblocks for subsampled data in 4:1:1
 * or 4:2:0 format.
 *
 * <buf>: a pointer to the data (3 color components)
 * if  interleaved=1, data is interleaved (c1c1c1c1 c2 c3, c1c1c1c1 c2 c3)
 * else it is not( data is in scan-line by scan -line format).
 *
 * width and heigh are already in multiples of 16; i.e. data
 * is already padded and subsampled as necessary.
 *
 * Some error checking should be done at the higher level
 * for the width height dimensions.
*/
  unsigned char *buf_ptr, *c1_ptr, *c2_ptr, *c3_ptr;
  int *block1, *block2, *block3, *block4;
  int *p1, *p2, *p3, *p4, i, j, k, l, t, skip, skip2,skip_row_data;
  int *p1_2, *p2_2;
  int nvMCU, nhMCU;  /* number of FULL MCU vert. and hor. */

  block1 = block2 = block3 = block4 = NULL; /* CHG_JPEG_MEM_FIX - for allocation checking */
  block1 = (int *) FPX_malloc(64 * sizeof(int));
  block2 = (int *) FPX_malloc(64 * sizeof(int));
  block3 = (int *) FPX_malloc(64 * sizeof(int));
  block4 = (int *) FPX_malloc(64 * sizeof(int));
  
  /* CHG_JPEG_MEM_FIX - added the following allocation check */
  if ((block1 == 0) || (block2 == 0) || (block3 == 0) || (block4 == 0)) {
    if (block1)   FPX_free(block1);
    if (block2)   FPX_free(block2);
    if (block3)   FPX_free(block3);
    return (EJPEG_ERROR_MEM);
  }

  Clear_Last_DC(jpeg_struct);

  nvMCU = height/16;
  nhMCU = width/16;

  /* Interleaved Data */
  if (interleaved == 1) {
    /* if interleaved,  a line has (width/2 * 6) pixels */
    skip = width*3 - 48;          /* 48 = 16 x 3 */
    skip_row_data = 16*3*width/2; /* One 16x16 block-row bytes */
    for (i=0; i < nvMCU; i++) {
      for (j=0; j < nhMCU; j++) {
        p3 = block3; 
        p4 = block4 ;    /* chroma blocks */
        buf_ptr = buf + i*skip_row_data + j*48;
        for (l=2; l>0; l--) {
          p1 = block1; 
          p2 = block2;  /* left-right blocks */
          p1_2 = p1 +8; 
          p2_2 = p2+8;
          for(k=4; k > 0; k--) {
            for(t=4; t > 0; t--) {
              *p1++ = *buf_ptr++ - SHIFT;
              *p1++ = *buf_ptr++ - SHIFT;
              *p1_2++ = *buf_ptr++ - SHIFT;
              *p1_2++ = *buf_ptr++ - SHIFT;
              *p3++ = *buf_ptr++ - SHIFT;
              *p4++ = *buf_ptr++ - SHIFT;
            } /* t-loop */
            for(t=4; t > 0; t--) {
              *p2++ = *buf_ptr++ - SHIFT;
              *p2++ = *buf_ptr++ - SHIFT;
              *p2_2++ = *buf_ptr++ - SHIFT;
              *p2_2++ = *buf_ptr++ - SHIFT;
              *p3++ = *buf_ptr++ - SHIFT;
              *p4++ = *buf_ptr++ - SHIFT;
            } /* t-loop */
            p1 = p1 + 8; 
            p2 = p2 +8;
            p1_2 = p1_2 + 8; 
            p2_2 = p2_2 +8;
            buf_ptr = buf_ptr + skip;
          } /* k-loop */
          EN_Encode_Block(block1, 0, &jpeg_struct->huff[0], &jpeg_struct->huff[1], 
              jpeg_struct->luminanceQuantization,jpeg_struct);
          EN_Encode_Block(block2, 0, &jpeg_struct->huff[0], &jpeg_struct->huff[1], 
              jpeg_struct->luminanceQuantization,jpeg_struct);
        }  /* end on l */
        EN_Encode_Block(block3, 1, &jpeg_struct->huff[2], &jpeg_struct->huff[3], 
            jpeg_struct->chrominanceQuantization,jpeg_struct);
        EN_Encode_Block(block4, 2, &jpeg_struct->huff[4], &jpeg_struct->huff[5], 
            jpeg_struct->tbl3_Quantization,jpeg_struct);
      } /* end on j */
    } /* end on i */
  } /* end of interleaved */
  /************************/
  /* Non-Interleaved Data */
  /************************/
  else {
    skip = width - 16;
    skip2 = (width/2) - 8;
    for(i = 0; i < nvMCU ; i++) {
      for(j = 0; j < nhMCU ; j++) {
        p3 = block3; 
        p4 = block4 ;    /* chroma blocks */
        c1_ptr = buf + j*16;
        c2_ptr = buf + width*height + j*8;
        c3_ptr = c2_ptr + (width*height)/4;
        for (l=2; l>0; l--) {
          p1 = block1; 
          p2 = block2;  /* left-right blocks */
          for(k=8; k > 0; k--) {
            /* Top */
            for(t=8; t > 0; t--) {
              *p1++ = *c1_ptr++ - SHIFT;
            } /* t-loop */
            /* bottom */
            for(t=8; t > 0; t--) {
              *p2++ = *c1_ptr++ - SHIFT;
            } /* t-loop */
            c1_ptr = c1_ptr + skip;
          } /* k-loop */
          EN_Encode_Block(block1, 0, &jpeg_struct->huff[0], &jpeg_struct->huff[1], 
              jpeg_struct->luminanceQuantization,jpeg_struct);
          EN_Encode_Block(block2, 0, &jpeg_struct->huff[0], &jpeg_struct->huff[1], 
              jpeg_struct->luminanceQuantization,jpeg_struct);
        }  /* end on l */
        /* Fill-up the chroma 8x8 blocks */
        for(k=8; k> 0; k--){
          for(t=8; t > 0; t--) {
            *p3++ = *c2_ptr++ - SHIFT;
            *p4++ = *c3_ptr++ - SHIFT;
          }
          c2_ptr = c2_ptr + skip2;
          c3_ptr = c3_ptr + skip2;
        } /* end on k*/
        EN_Encode_Block(block3, 1, &jpeg_struct->huff[2], &jpeg_struct->huff[3], 
            jpeg_struct->chrominanceQuantization,jpeg_struct);
        EN_Encode_Block(block4, 2, &jpeg_struct->huff[4], &jpeg_struct->huff[5], 
            jpeg_struct->tbl3_Quantization,jpeg_struct);
      } /* end on j */
    } /* end on i */
  } /* end of non-interleaved data. */
  FPX_free(block1);
  FPX_free(block2);
  FPX_free(block3);
  FPX_free(block4);

  return(0);
}

/*
 * Return 0 if successful, ERROR_FORMAT if unsupported format 
 */
int EN_Encode_Scan(TILE_DATA *tile_data,
JPEG_STRUCT *jpeg_struct)
{
  int width, height,components;
  int hdim[4],vdim[4],i;
  int interleave_flag;

  components = tile_data->components;
  width = tile_data->width;
  height = tile_data->height;
  interleave_flag = tile_data->iflag;
  for (i=0; i < 4 ;i++) {
    hdim[i] = tile_data->Hsamp[i];
    vdim[i] = tile_data->Vsamp[i];
  }

  if (components == 1) {
    return(EN_Encode_Scan_Gray
        (tile_data->data,width,height,jpeg_struct));
  } else if ( (components == 2) && 
      (hdim[0] == 1) && (hdim[1] == 1) && 
      (vdim[0] == 1) && (vdim[1] == 1) ) {
    return(EN_Encode_Scan_Color11(tile_data->data, width, height, 
        interleave_flag, 
        jpeg_struct));
  } else if ( (components == 3) && 
      (hdim[0] == 2) && (hdim[1] == 1) && (hdim[2] == 1) &&
      (vdim[0] == 2) && (vdim[1] == 1) && (vdim[2] == 1) ) {
    return(EN_Encode_Scan_Color411(tile_data->data, 
        width, height, 
        interleave_flag, 
        jpeg_struct));
  } else if ( (components == 3) && 
      (hdim[0] == 2) && (hdim[1] == 1) && (hdim[2] == 1) &&
      (vdim[0] == 1) && (vdim[1] == 1) && (vdim[2] == 1) ) {
    return(EN_Encode_Scan_Color422(tile_data->data, 
        width, height, 
        interleave_flag, 
        jpeg_struct));
  } else if ( (components == 3) && 
      (hdim[0] == 1) && (hdim[1] == 1) && (hdim[2] == 1) &&
      (vdim[0] == 1) && (vdim[1] == 1) && (vdim[2] == 1) ) {
    return(EN_Encode_Scan_Color111(tile_data->data, width, height, 
        interleave_flag, 
        jpeg_struct));
  } else if ( (components == 4) && 
      (hdim[0] == 1) && (hdim[1] == 1) && (hdim[2] == 1)
      && (hdim[3] == 1) &&
      (vdim[0] == 1) && (vdim[1] == 1) && (vdim[2] == 1)
      && (vdim[3] == 1) ) {
    return(EN_Encode_Scan_Color1111(tile_data->data, width, height, 
        interleave_flag, 
        jpeg_struct));
  } else if ( (components == 4) && 
      (hdim[0] == 2) && (hdim[1] == 1) && (hdim[2] == 1)
      && (hdim[3] == 2) &&
      (vdim[0] == 2) && (vdim[1] == 1) && (vdim[2] == 1)
      && (vdim[3] == 2) ) {
    return(EN_Encode_Scan_Color4114(tile_data->data, width, height, 
        interleave_flag, 
        jpeg_struct));
  } else if ( (components == 4) && 
      (hdim[0] == 2) && (hdim[1] == 1) && (hdim[2] == 1)
      && (hdim[3] == 2) &&
      (vdim[0] == 1) && (vdim[1] == 1) && (vdim[2] == 1)
      && (vdim[3] == 1) ) {
    return(EN_Encode_Scan_Color4224(tile_data->data, width, height, 
        interleave_flag, 
        jpeg_struct));
  } else {
    return(EJPEG_ERROR_FORMAT);
  }
}


/*
* Return 0 if successful, -1 otherwise.
*/
int JPEGEncodeTileInit(
unsigned char *data,  /* points to the input tile */
int width, 
int height,
int bytes,    /* number of bytes per pixel */
int quality_factor,
int *Hsamp,int *Vsamp, /* Hor. & Vert. subsampling factors */
int iflag,    /* 1/0 = Interleaved/Non-interleaved data */
TILE_DATA *tile_data,
int nu_huff,  /* # of distinct Huffman Tables (a max of 8 tables: four DC-AC pairs) */
JPEGHuffTable *huffman_table,
unsigned char *CompDCHuffIdent,
unsigned char *CompACHuffIdent,
int nu_qtables, /* # of distinct Q-tables (a max of 4 tables) */
JPEGQuantTable *q_table,
unsigned char *CompQuantIdent,
JPEG_STRUCT *jpeg_struct,
unsigned char *header_buffer,
long header_buffer_size,
long *header_bytes)
{
   int Iflag;

  EB_Init(header_buffer,header_buffer_size);

  if (EP_Begin()) {
    return(EJPEG_ERROR_MEM);
  } else {
    EP_Write_SOI();
    {
      unsigned char *hclass, *ident, **bits, **huffval;
      int i,j;
      int q[4][64], *intptr, *intdstptr = 0, whereIndex[4];
      int whereDCIndex[4],whereACIndex[4];

      /* Scale input Quantization matrices by the Q-factor */
      if (quality_factor < 1)
        quality_factor = 1;
      else if (quality_factor > MAX_Q)
        quality_factor = 255;

      for (i=0; i < nu_qtables ;i++) {
        whereIndex[q_table[i].ident]=i;
        Scale_Char_Matrix(quality_factor, Q_PRECISION,
            q_table[i].quantizer,q[i]);
        EP_Write_DQT(0, q_table[i].ident, q[i]);
        Fill_Winograd_Quant_Table(q[i],q[i]);
      }
      for (i=0; i < bytes ;i++) {
        if (i==0) {
          intdstptr = jpeg_struct->luminanceQuantization;
        }
        else if (i==1) {
          intdstptr = jpeg_struct->chrominanceQuantization;
        }
        else if (i==2) {
          intdstptr = jpeg_struct->tbl3_Quantization;
        }
        else if (i==3) {
          intdstptr = jpeg_struct->tbl4_Quantization;
        }
        intptr = q[whereIndex[CompQuantIdent[i]]];
        for (j=0; j < 64 ; j++) {
          *intdstptr++ = *intptr++;
        }
      }

      /* Read in Huffman Tables */
      bits = huffval = NULL;      /* CHG_JPEG_MEM_FIX - for allocation checking */
      hclass = ident = NULL;      /* CHG_JPEG_MEM_FIX - for allocation checking */
      bits   =(unsigned char **) FPX_malloc(8 * sizeof(unsigned char *));
      huffval=(unsigned char **) FPX_malloc(8 * sizeof(unsigned char *));
      hclass =(unsigned char *) FPX_malloc(8 * sizeof(unsigned char));
      ident  =(unsigned char *) FPX_malloc(8 * sizeof(unsigned char));
  
      /* CHG_JPEG_MEM_FIX - added the following allocation check */
      if ((bits == 0) || (huffval == 0) || (hclass == 0) || (ident == 0)) {
        if (bits)   FPX_free(bits);
        if (huffval)  FPX_free(huffval);
        if (hclass)   FPX_free(hclass);
        if (ident)    FPX_free(ident);
        return (EJPEG_ERROR_MEM);
      }

      for (i=0; i < nu_huff ;i++) {
        bits[i] = huffman_table[i].bits;
        huffval[i] = huffman_table[i].vals;
        hclass[i]  = huffman_table[i].hclass;
        ident[i]   = huffman_table[i].ident;
      }
      EP_Write_DHTs(nu_huff, hclass, ident, bits, huffval);

      /* The Huffman Tables alternate as: DC-AC-DC-AC-... */
      for (i=0; i < nu_huff ;i +=2) {
        whereDCIndex[huffman_table[i].ident]=i;
        whereACIndex[huffman_table[i+1].ident]=i+1;
      }

      /* Set up internal structures */
      for (i=0; i < bytes ;i++) {
        BuildHuffmanTable(huffman_table[whereDCIndex[CompDCHuffIdent[i]]].bits, 
            huffman_table[whereDCIndex[CompDCHuffIdent[i]]].vals,
            &jpeg_struct->huff[i*2]);
        BuildHuffmanTable(huffman_table[whereACIndex[CompACHuffIdent[i]]].bits, 
            huffman_table[whereACIndex[CompACHuffIdent[i]]].vals,
            &jpeg_struct->huff[i*2+1]);
      }

      FPX_free(hclass);
      FPX_free(ident);
      FPX_free(bits);
      FPX_free(huffval);
    }
    EP_Write_EOI();  /* Create an Abbreviated-Format for table-specification data */
    EP_End();
  }

  /* Set up internal structures */
  tile_data->width = width;
  tile_data->height = height;
  tile_data->components = bytes;
  Iflag = (iflag==0) ? 1 : 0;
  tile_data->iflag = Iflag; /* interleaved data */
  tile_data->data = data;
  tile_data->Hsamp=Hsamp;
  tile_data->Vsamp=Vsamp;

  EB_End(header_bytes);

  return(0);
}

/*
* Return 0 if successful, -1 otherwise.
*/
int JPEGEncodeTile(
TILE_DATA *tile_data,
JPEG_STRUCT *jpeg_struct,
unsigned char *CompDCHuffIdent,
unsigned char *CompACHuffIdent,
unsigned char *CompQuantIdent,
unsigned char *out_buf,
long int out_buf_size,
long int *compr_size)
{
  int width,height,components;

  width  = tile_data->width;
  height = tile_data->height;
  components  = tile_data->components;

  EB_Init(out_buf, out_buf_size);
  EP_Begin();
  EP_Write_SOI();  /* Abbreviated format for compressed image data */
  if (EP_Write_SOF((int)width,(int)height,
      tile_data->Hsamp,tile_data->Vsamp,(int)(components),
      CompQuantIdent)){
    EP_End();
    return(EJPEG_ERROR_MEM);
  }
  EP_Write_SOS(components,CompDCHuffIdent,CompACHuffIdent);
  if (EN_Encode_Scan(tile_data, jpeg_struct)) {
    EP_End();
    return(EJPEG_ERROR_EOF);
  }
  EP_Write_EOI();
  EP_End();
  EB_End(compr_size);
  return(0);
}
