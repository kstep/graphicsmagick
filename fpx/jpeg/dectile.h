/* -*-C-*-
******************************************************************************
*
* File:         dectile.h
* Description:  see dectile.c
* Author:       Kirt Winter
* Created:      Fri Aug  4 11:04:22 1995
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

#ifndef _DECTILE_H
#define _DECTILE_H

#ifndef TILE_DATA_DEFINED
typedef struct {
  int width;     /* tile width */
  int height;    /* tile height */
  int components; /* # of components: 1-4 */
  int *Hsamp, *Vsamp;  /* one pair of <Hsamp,Vsamp> subsampling factors per component */
  int iflag;     /* Interleave-flag: 1/0= Interleaved/Non-interleaved */
  unsigned char *data; /* points to image/compressed data */
} TILE_DATA;
#define TILE_DATA_DEFINED  
#endif

JPEGEXPORT
int DE_Decode(DB_STATE *db_state,
           void *decoder,
        int method,   /* 0: Chen DCT */
        int gray_scale, /* if TRUE, color JPEG image will be decoded as grayscale */
        int pass_num, /* which pass (1 = 1st or 2 = 2nd) */
        int *image_w,
        int *image_h,
        int *bytes_per_pixel,
        int *hdim,
        int *vdim,
        int interleave);  

JPEGEXPORT
int  JPEGDecodeTileInit( unsigned char *inbuf,
            size_t inbuf_size,
            TILE_DATA *tile_data,
            size_t *header_size,
            void *decoder);

void warning(int type);

#endif /* _DECTILE_H */
