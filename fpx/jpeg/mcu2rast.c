/* -*-C-*-
******************************************************************************
*
* File:         mcu2rast.c
* RCS:          $Header$
* Description:  Converts the MCU data into raster order pixels.
* Author:       Gregory S. Yovanof
* Created:      Thu Jul 13 11:04:22 1996
* Initial Source Release: Mon Jan 22 1996
* Language:     C
* Package:      Hewlett-Packard JPEG Encoder/Decoder
*
* Copyright (c) 1999 Digital Imaging Group, Inc.
* For conditions of distribution and use, see copyright notice
* in Flashpix.h
*
******************************************************************************
*/

#ifdef _WINDOWS
#define WINDLL
#endif

#include <stdio.h>
#include "jpegconf.h"
#include "decoder.h"
#include "dbuffer.h"
#include "dectile.h"
#include "djpeg.h"

void Write_Scan_MCUs_Mono(
unsigned char *outbuf, /* image out */
int *MCUbuf, /* sequence of MCUs */
int width,   /* image width */
int height  /* image height */
)
{
  int i,j, skip, nvblocks,nhblocks;
  int k,t;
  int *inptr;
  unsigned char *ptr;

  inptr = MCUbuf;

  skip = width-8;
  nvblocks = height/8;
  nhblocks = width/8;
  for (i=0; i < nvblocks; i++) {
    for(j=0; j < nhblocks; j++) {
      ptr = outbuf + i*8*width + j*8;
      for(k = 8; k > 0; k--) {
        for(t = 8; t > 0; t--) {
          *ptr++ = (unsigned char) *inptr++ ;
        }
        ptr = ptr + skip;
      }
    }
  }
}

void Write_Scan_MCUs_211(
unsigned char *outbuf, /* image out */
int *MCUbuf, /* sequence of MCUs */
int width,   /* image width */
int height,  /* image height */
int interleave  /* Interleave flag */
)
{
/* 
 * Routine to generate raster scan data from MCUs.
 * Case: subsampled data in 4:1:1
 * or 4:2:0 format.
 *
 * <MCUbuf>: a pointer to the data 
 * if  interleaved=1, output data is interleaved (c1c1c1c1 c2 c3, 
   c1c1c1c1 c2 c3)
 * else it is not( data is in scan-line by scan -line format).
 *
 * width and heigh are already in multiples of 16; i.e. data
 * is already padded and subsampled as necessary.
 *
*/
  unsigned char *buf_ptr, *c2_ptr, *c3_ptr;
  int *p1, *p2, *p3, *p4, i, j, k, l, t, skip, skip2,skip_row_data;
  int *p1_2, *p2_2;
  int *mcupos;
  int nvMCU, nhMCU;  /* number of FULL MCU vert. and hor. */

  nvMCU = height/16;
  nhMCU = width/16;

  /* Interleaved Data */
  if (interleave == 1) {
    /* if interleaved,  a line has (width/2 * 6) pixels 
    */
    skip = width*3 - 48;          /* 48 = 16 x 3 */
    skip_row_data = 24*width; /* One 16x16 block-row bytes */
    /* 24 = 16*3bytes*6/12 */
    for (i = 0; i < nvMCU; i++) {
      for (j=0; j < nhMCU; j++) {
        mcupos = MCUbuf+(i*nhMCU+j)*384;
        p3 = mcupos+4*64;    /* chroma blocks */
        p4 = mcupos+5*64;
        buf_ptr = outbuf + i*skip_row_data + j*48;
        for (l=0; l<2; l++) {
          if(l == 0) {
            p1 =  mcupos; /* left-right blocks */
          }
          else {
            p1 = mcupos+128;
          }
          p2 = p1+64;
          p1_2 = p1 +8; 
          p2_2 = p2+8;
          for(k=4; k > 0; k--) {
            for(t=4; t > 0; t--) {
              *buf_ptr++ = (unsigned char) *p1++;
              *buf_ptr++ = (unsigned char) *p1++;
              *buf_ptr++ = (unsigned char) *p1_2++ ;
              *buf_ptr++ = (unsigned char) *p1_2++ ;
              *buf_ptr++ = (unsigned char) *p3++ ;
              *buf_ptr++ = (unsigned char) *p4++ ;
            } /* t-loop */
            for(t=4; t > 0; t--) {
              *buf_ptr++ = (unsigned char) *p2++ ;
              *buf_ptr++ = (unsigned char) *p2++ ;
              *buf_ptr++ = (unsigned char) *p2_2++;
              *buf_ptr++ = (unsigned char) *p2_2++;
              *buf_ptr++ = (unsigned char) *p3++ ;
              *buf_ptr++ = (unsigned char) *p4++ ;
            } /* t-loop */
            p1 = p1 + 8; 
            p2 = p2 +8;
            p1_2 = p1_2 + 8; 
            p2_2 = p2_2 +8;
            buf_ptr = buf_ptr + skip;
          } /* k-loop */
        }  /* end on l */
      } /* end on j */
    } /* end on i */
  } /* end of interleaved */
  /************************/
  /* Non-Interleaved Data */
  /************************/
  else {
    /* if interleaved,  a line has "width" pixels */
    skip = width - 16;
    skip2 = (width/2) - 8;
    skip_row_data = 16*width; /* One row of MCUs */

    for (i = 0; i < nvMCU; i++) {
      for (j=0; j < nhMCU; j++) {
        mcupos = MCUbuf+(i*nhMCU+j)*384;
        p3 = mcupos+4*64;    /* chroma blocks */
        p4 = mcupos+5*64;
        /* Where the luma and chroma will be stored */
        buf_ptr = outbuf + i*skip_row_data + j*16;
        c2_ptr = outbuf+width*height+j*8+i*width*4;
        c3_ptr = c2_ptr + (width*height/4);

        for (l=0; l<2; l++) {
          if(l == 0) {
            p1 =  mcupos; /* left-right blocks */
          }
          else {
            p1 = mcupos+128;
          }
          p2 = p1+64;
          for(k=8; k > 0; k--) {
            for(t=8; t > 0; t--) {
              *buf_ptr++ = (unsigned char) *p1++;
            } /* t-loop */
            for(t=8; t > 0; t--) {
              *buf_ptr++ = (unsigned char) *p2++;
            } /* t-loop */
            buf_ptr = buf_ptr + skip;
          } /* k-loop */
        }  /* end on l */
        /* fill the chroma now */
        for(k =8; k>0; k--) {
          for(t=8; t>0; t--) {
            *c2_ptr++ = (unsigned char) *p3++;
            *c3_ptr++ = (unsigned char) *p4++;
          }
          c2_ptr = c2_ptr +skip2;
          c3_ptr = c3_ptr +skip2;
        } /* end on k */
      } /* end on j */
    } /* end on i */
  } /* end of non-interleaved */
}

void Write_Scan_MCUs_422(
unsigned char *outbuf, /* image out */
int *MCUbuf, /* sequence of MCUs */
int width,   /* image width */
int height,  /* image height */
int interleave  /* Interleave flag */
)
{
/* 
 * Routine to generate raster scan data from MCUs.
 * Case: subsampled data in 4:2:2
 *
 * <MCUbuf>: a pointer to the data 
 * if  interleaved=1, output data is interleaved (c1c1 c2 c3, 
   c1c1 c2 c3)
 * else it is not( data is in scan-line by scan-line format).
 *
 * width and height are already in multiples of 8 or 16; i.e. data
 * is already padded and subsampled as necessary.
 *
*/
  unsigned char *buf_ptr, *c2_ptr, *c3_ptr;
  int *p1, *p2, *p3, *p4, i, j, k, t;
  int skip, skip2, skip_row_data, skip_row_data2;
  int *mcupos;
  int nvMCU, nhMCU;  /* number of FULL MCU vert. and hor. */

  nvMCU = height/8;
  nhMCU = width/16;

  /* Interleaved Data */
  if (interleave == 1) {
    /* if interleaved,  a line has (width/2 * 4) pixels 
    */
    skip = width*2 - 32; /* 32 = 8 subsampled symbols x 4bytes/ss-symbol */
    skip_row_data = width * 16; /* One 8x16 block-row bytes */
    /* 16 = 8*4*width/2 */
    for (i = 0; i < nvMCU; i++) {
      for (j=0; j < nhMCU; j++) {
        mcupos = MCUbuf+(i*nhMCU+j)*256; /* 4 blocks/MCU */
        buf_ptr = outbuf + i*skip_row_data + j*32;

        p1 =  mcupos; /* left-right blocks */
        p2 = p1+64;
        p3 = mcupos+2*64;    /* chroma blocks */
        p4 = p3 + 64;
        for(k=8; k > 0; k--) { /* lines in MCU */
          for(t=4; t > 0; t--) {
            *buf_ptr++ = (unsigned char) *p1++;
            *buf_ptr++ = (unsigned char) *p1++;
            *buf_ptr++ = (unsigned char) *p3++ ;
            *buf_ptr++ = (unsigned char) *p4++ ;
          } /* t-loop */
          for(t=4; t > 0; t--) {
            *buf_ptr++ = (unsigned char) *p2++ ;
            *buf_ptr++ = (unsigned char) *p2++ ;
            *buf_ptr++ = (unsigned char) *p3++ ;
            *buf_ptr++ = (unsigned char) *p4++ ;
          } /* t-loop */
          /* p1 = p1 + 8; p2 = p2 +8; GSY 5/6/96 */
          buf_ptr = buf_ptr + skip;
        } /* k-loop */

      } /* end on j */
    } /* end on i */
  } /* end of interleaved */
  /************************/
  /* Non-Interleaved Data */
  /************************/
  else {
    /* if interleaved,  a line has "width" pixels */
    skip = width - 16;
    skip2 = (width/2) - 8;
    skip_row_data = 8*width; /* One row of MCUs */
    skip_row_data2 = 8*width/2;  /* one row of chroma blocks */

    for (i = 0; i < nvMCU; i++) {
      for (j=0; j < nhMCU; j++) {
        mcupos = MCUbuf+(i*nhMCU+j)*256;
        p3 = mcupos+4*64;    /* chroma blocks */
        p4 = p3 + 64;
        /* Where the luma and chroma will be stored */
        buf_ptr = outbuf + i*skip_row_data + j*16;
        c2_ptr = outbuf+width*height+j*8+i*width*4;
        c3_ptr = c2_ptr + (width*height/4);

        p1 =  mcupos; /* left-right blocks */
        p2 = p1+64;
        for(k=8; k > 0; k--) {
          for(t=8; t > 0; t--) {
            *buf_ptr++ = (unsigned char) *p1++;
          } /* t-loop */
          for(t=8; t > 0; t--) {
            *buf_ptr++ = (unsigned char) *p2++;
          } /* t-loop */
          buf_ptr = buf_ptr + skip;
        } /* k-loop */

        /* fill the chroma now */
        for(k =8; k>0; k--) {
          for(t=8; t>0; t--) {
            *c2_ptr++ = (unsigned char) *p3++;
            *c3_ptr++ = (unsigned char) *p4++;
          }
          c2_ptr = c2_ptr +skip2;
          c3_ptr = c3_ptr +skip2;
        } /* end on k */
      } /* end on j */
    } /* end on i */
  } /* end of non-interleaved */
}

void Write_Scan_MCUs_4224(
unsigned char *outbuf, /* image out */
int *MCUbuf, /* sequence of MCUs */
int width,   /* image width */
int height,  /* image height */
int interleave  /* Interleave flag */
)
{
/* 
 * Routine to generate raster scan data from MCUs.
 * Case: subsampled data in 4:2:2:4 format
 *
 * <MCUbuf>: a pointer to the data 
 * if  interleaved=1, output data is interleaved (c1c1 c2 c3 c4c4, 
   c1c1 c2 c3 c4c4)
 * else it is not( data is in scan-line by scan-line format).
 *
 * width and height are already in multiples of 8 or 16; i.e. data
 * is already padded and subsampled as necessary.
 *
*/
  unsigned char *buf_ptr, *c1_ptr, *c2_ptr, *c3_ptr, *c4_ptr;
  int *p1, *p2, *p3, *p4, *p5, *p6, i, j, k, t;
  int skip, skip2, skip_row_data, skip_row_data2;
  int *mcupos;
  int nvMCU, nhMCU;  /* number of FULL MCU vert. and hor. */

  nvMCU = height/8;
  nhMCU = width/16;

  /* Interleaved Data */
  if (interleave == 1) {
    /* if interleaved,  a line has (width/2 * 6) pixels (1byte/pixel) */
    /* A subsampled symbol has 6 bytes. */
    skip = width*3 - 48; /* 48 = 8 subsampled symbols x 6bytes/ss-symbol */
    skip_row_data = width * 24; /* One 8x16 block-row bytes */
    /* 8 lines * (6*width/2)bytes/line */
    for (i = 0; i < nvMCU; i++) {
      for (j=0; j < nhMCU; j++) {
        mcupos = MCUbuf+(i*nhMCU+j)*384; /* 6 blocks/MCU */
        buf_ptr = outbuf + i*skip_row_data + j*48;

        p1 = mcupos; /* left-right blocks */
        p2 = p1+64;
        p3 = p2+64;    /* chroma blocks */
        p4 = p3+64;
        p5 = p4+64;
        p6 = p5+64;
        for(k=8; k > 0; k--) { /* lines in MCU */
          for(t=4; t > 0; t--) {
            *buf_ptr++ = (unsigned char) *p1++;
            *buf_ptr++ = (unsigned char) *p1++;
            *buf_ptr++ = (unsigned char) *p3++ ;
            *buf_ptr++ = (unsigned char) *p4++ ;
            *buf_ptr++ = (unsigned char) *p5++;
            *buf_ptr++ = (unsigned char) *p5++;
          } /* t-loop */
          for(t=4; t > 0; t--) {
            *buf_ptr++ = (unsigned char) *p2++ ;
            *buf_ptr++ = (unsigned char) *p2++ ;
            *buf_ptr++ = (unsigned char) *p3++ ;
            *buf_ptr++ = (unsigned char) *p4++ ;
            *buf_ptr++ = (unsigned char) *p6++ ;
            *buf_ptr++ = (unsigned char) *p6++ ;
          } /* t-loop */
          buf_ptr = buf_ptr + skip;
        } /* k-loop */

      } /* end on j */
    } /* end on i */
  } /* end of interleaved */
  /************************/
  /* Non-Interleaved Data */
  /************************/
  else {
    /* if interleaved,  a line has "width" pixels */
    skip = width - 16;
    skip2 = (width/2) - 8;
    skip_row_data = 8*width; /* One row of MCUs */
    skip_row_data2 = 8*width/2;  /* one row of chroma blocks */

    for (i = 0; i < nvMCU; i++) {
      for (j=0; j < nhMCU; j++) {
        mcupos = MCUbuf+(i*nhMCU+j)*384;
        p1 =  mcupos; /* left-right blocks */
        p2 = p1+64;
        p3 = mcupos+4*64;    /* chroma blocks */
        p4 = p3 + 64;
        p5 = p4 + 64;
        p6 = p5 + 64;
        /* Where the luma, chroma and alpha will be stored */
        c1_ptr = outbuf + i*skip_row_data + j*16;
        c2_ptr = outbuf+width*height+j*8+i*width*4;
        c3_ptr = c2_ptr + (width*height/4);
        c4_ptr = outbuf + 3*width*height/2 + i*skip_row_data + j*16;

        for(k=8; k > 0; k--) {
          for(t=8; t > 0; t--) {
            *c1_ptr++ = (unsigned char) *p1++;
            *c4_ptr++ = (unsigned char) *p5++;
          } /* t-loop */
          for(t=8; t > 0; t--) {
            *c1_ptr++ = (unsigned char) *p2++;
            *c4_ptr++ = (unsigned char) *p6++;
          } /* t-loop */
          c1_ptr = c1_ptr + skip;
        } /* k-loop */

        /* fill the chroma now */
        for(k =8; k>0; k--) {
          for(t=8; t>0; t--) {
            *c2_ptr++ = (unsigned char) *p3++;
            *c3_ptr++ = (unsigned char) *p4++;
          }
          c2_ptr = c2_ptr +skip2;
          c3_ptr = c3_ptr +skip2;
        } /* end on k */
      } /* end on j */
    } /* end on i */
  } /* end of non-interleaved */
}

void Write_Scan_MCUs_111(
unsigned char *outbuf, /* image out */
int *MCUbuf, /* sequence of MCUs */
int width,   /* image width */
int height,  /* image height */
int interleave  /* Interleave flag */
)
/* 
 * Routine to generate raster scan data from macroblocks 
   for NON-subsampled data (1:1:1)
 * MCUbuf: a pointer to the MCUs
 * if  interleaved=1, data output is interleaved (RGB RGB ..)
 * else it is not.
 *
*/
{
  unsigned char *buf_ptr;
  int *p1, *p2, *p3, i, j, k, t, skip, block_row_skip;
  int nvMCU, nhMCU;         /* number of FULL MCU vert. and hori. */
  unsigned char *c1, *c2, *c3;
  int *mcupos;

  nvMCU = (height)/8;
  nhMCU = (width)/8;

  if(interleave == 1) {
    skip = (3*width) - 24;
    block_row_skip = 3*width*8;
    for (i = 0; i < nvMCU; i++) {
      for (j = 0; j < nhMCU; j++) {
        mcupos = MCUbuf + (i*nhMCU+j)*192;

        p1 = mcupos;
        p2 = mcupos+64;
        p3 = p2 + 64;
        buf_ptr = outbuf + i*block_row_skip + j*24;
        for (k = 8; k > 0; k--) {
          for (t = 8; t > 0; t--) {
            *buf_ptr++ = (unsigned char) *p1++ ;
            *buf_ptr++ = (unsigned char) *p2++ ;
            *buf_ptr++ = (unsigned char) *p3++ ;
          }
          buf_ptr = buf_ptr + skip;
        }
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
        mcupos = MCUbuf + (i*nhMCU+j)*192;

        p1 = mcupos;
        p2 = mcupos+64;
        p3 = p2 + 64;

        c1 = outbuf + i*block_row_skip + j*8;
        c2 = c1 + width * height;
        c3 = c2 + width * height;
        for (k = 8; k > 0; k--) {
          for (t = 8; t > 0; t--) {
            *c1++ = (unsigned char) *p1++ ;
            *c2++ = (unsigned char) *p2++ ;
            *c3++ = (unsigned char) *p3++ ;
          }
          c1 = c1 + skip;
          c2 = c2 + skip;
          c3 = c3 + skip;
        }
      } /* end of j */
    } /* end of i */
  } /* end of else loop */
}

void Write_Scan_MCUs_4114(
unsigned char *outbuf, /* image out */
int *MCUbuf, /* sequence of MCUs */
int width,   /* image width */
int height,  /* image height */
int interleave  /* Interleave flag */
)
{
/* 
 * Routine to generate raster scan data from MCUs.
 * Case: subsampled data in 4:1:1:4
 * i.e., 4 channel data with the middle 2 channels
 * being subsampled by 2x in both dimensions.
 *
 * <MCUbuf>: a pointer to the data 
 * if  interleaved=1, output data is interleaved (c1c1c1c1 c2 c3, 
   c1c1c1c1 c2 c3)
 * else it is not( data is in scan-line by scan-line format).
 *
 * width and heigh are already in multiples of 16; i.e. data
 * is already padded and subsampled as necessary.
 *
*/
  unsigned char *buf_ptr, *c1_ptr, *c2_ptr, *c3_ptr;
  int *p1, *p2, *p3, *p4, i, j, k, l, t, skip, skip2,skip_row_data;
  int *p1_2, *p2_2;
  int *mcupos;
  int nvMCU, nhMCU;  /* number of FULL MCU vert. and hor. */
  int *p5,*p6,*p5_2,*p6_2;
  unsigned char *c4_ptr;

  nvMCU = height/16;
  nhMCU = width/16;

  /* Interleaved Data */
  if (interleave == 1) {
    /* if interleaved, a line has (width/2 * 10) pixels */
    skip = width*5 - 80;  /* 80 = 8 subsampled blocks * 10 bytes/block  */
    skip_row_data = 40*width;  /* One 16x16 block-row bytes */
    /* 40 = 16rows * 4 bytes * 10/16 (MCU reduction) */
    for (i = 0; i < nvMCU; i++) {
      for (j=0; j < nhMCU; j++) {
        mcupos = MCUbuf+(i*nhMCU+j)*640;
        /* 640 = 10 blocks/MCU * 64 bytes */
        p3 = mcupos+4*64;    /* chroma blocks */
        p4 = mcupos+5*64;
        buf_ptr = outbuf + i*skip_row_data + j*80;
        for (l=0; l<2; l++) {
          if(l == 0) {
            p1 =  mcupos; /* left-right blocks */
          }
          else {
            p1 = mcupos+128;
          }
          p2 = p1+64;
          p1_2 = p1+8; 
          p2_2 = p2+8;
          p5 = p1+6*64; 
          p6 = p5+64;
          p5_2 = p5+8; 
          p6_2 = p6+8;
          for(k=4; k > 0; k--) {
            for(t=4; t > 0; t--) {
              *buf_ptr++ = (unsigned char) *p1++;
              *buf_ptr++ = (unsigned char) *p1++;
              *buf_ptr++ = (unsigned char) *p1_2++ ;
              *buf_ptr++ = (unsigned char) *p1_2++ ;
              *buf_ptr++ = (unsigned char) *p3++ ;
              *buf_ptr++ = (unsigned char) *p4++ ;
              *buf_ptr++ = (unsigned char) *p5++;
              *buf_ptr++ = (unsigned char) *p5++;
              *buf_ptr++ = (unsigned char) *p5_2++ ;
              *buf_ptr++ = (unsigned char) *p5_2++ ;
            } /* t-loop */
            for(t=4; t > 0; t--) {
              *buf_ptr++ = (unsigned char) *p2++ ;
              *buf_ptr++ = (unsigned char) *p2++ ;
              *buf_ptr++ = (unsigned char) *p2_2++;
              *buf_ptr++ = (unsigned char) *p2_2++;
              *buf_ptr++ = (unsigned char) *p3++ ;
              *buf_ptr++ = (unsigned char) *p4++ ;
              *buf_ptr++ = (unsigned char) *p6++ ;
              *buf_ptr++ = (unsigned char) *p6++ ;
              *buf_ptr++ = (unsigned char) *p6_2++;
              *buf_ptr++ = (unsigned char) *p6_2++;
            } /* t-loop */
            p1 = p1 + 8; 
            p2 = p2 +8;
            p1_2 = p1_2 + 8; 
            p2_2 = p2_2 +8;
            p5 += 8; 
            p6 += 8;
            p5_2 += 8; 
            p6_2 += 8;
            buf_ptr = buf_ptr + skip;
          } /* k-loop */
        }  /* end on l */
      } /* end on j */
    } /* end on i */
  } /* end of interleaved */
  /************************/
  /* Non-Interleaved Data */
  /************************/
  else {
    /* if non-interleaved, a line has "width" pixels */
    skip = width - 16;
    skip2 = (width/2) - 8;
    skip_row_data = 16*width; /* One row of MCUs */

    for (i = 0; i < nvMCU; i++) {
      for (j=0; j < nhMCU; j++) {
        mcupos = MCUbuf+(i*nhMCU+j)*640;
        p3 = mcupos+4*64;    /* chroma blocks */
        p4 = mcupos+5*64;
        /* Where the luma and chroma will be stored */
        c1_ptr = outbuf + i*skip_row_data + j*16;
        c2_ptr = outbuf+width*height+j*8+i*width*4;
        c3_ptr = c2_ptr + (width*height/4);
        c4_ptr = c1_ptr + (width*height/2);

        for (l=0; l<2; l++) { /* output luma  & alpha-channel */
          if(l == 0) {
            p1 =  mcupos; /* left-right blocks */
          }
          else {
            p1 = mcupos+128;
          }
          p2 = p1+64;
          p5 = p1+6*64; 
          p6 = p5+64;
          for(k=8; k > 0; k--) {
            for(t=8; t > 0; t--) {
              *c1_ptr++ = (unsigned char) *p1++;
              *c4_ptr++ = (unsigned char) *p5++;
            } /* t-loop */
            for(t=8; t > 0; t--) {
              *c1_ptr++ = (unsigned char) *p2++;
              *c4_ptr++ = (unsigned char) *p6++;
            } /* t-loop */
            c1_ptr = c1_ptr + skip;
            c4_ptr = c4_ptr + skip;
          } /* k-loop */
        }  /* end on l */
        /* fill the chroma now */
        for(k =8; k>0; k--) {
          for(t=8; t>0; t--) {
            *c2_ptr++ = (unsigned char) *p3++;
            *c3_ptr++ = (unsigned char) *p4++;
          }
          c2_ptr = c2_ptr +skip2;
          c3_ptr = c3_ptr +skip2;
        } /* end on k */
      } /* end on j */
    } /* end on i */
  } /* end of non-interleaved */
}

void Write_Scan_MCUs_1111(
unsigned char *outbuf, /* image out */
int *MCUbuf, /* sequence of MCUs */
int width,   /* image width */
int height,  /* image height */
int interleave  /* Interleave flag */
)
/* 
 * Routine to generate raster scan data from macroblocks 
   for NON-subsampled data with four components (1:1:1:1)
 * MCUbuf: a pointer to the MCUs
 * if  interleaved=1, data output is interleaved (eg. MCYK, MCYK,...)
 * else it is not.
 *
*/
{
  unsigned char *buf_ptr;
  int *p1, *p2, *p3, *p4, i, j, k, t, skip, block_row_skip;
  int nvMCU, nhMCU;         /* number of FULL MCU vert. and hori. */
  unsigned char *c1, *c2, *c3, *c4;
  int *mcupos;

  nvMCU = (height)/8;
  nhMCU = (width)/8;

  if(interleave == 1) {
    skip = (4*width) - 32;
    block_row_skip = 4*width*8;
    for (i = 0; i < nvMCU; i++) {
      for (j = 0; j < nhMCU; j++) {
        mcupos = MCUbuf + (i*nhMCU+j)*256;

        p1 = mcupos;
        p2 = mcupos+64;
        p3 = p2 + 64;
        p4 = p3 + 64;
        buf_ptr = outbuf + i*block_row_skip + j*32;
        for (k = 8; k > 0; k--) {
          for (t = 8; t > 0; t--) {
            *buf_ptr++ = (unsigned char) *p1++ ;
            *buf_ptr++ = (unsigned char) *p2++ ;
            *buf_ptr++ = (unsigned char) *p3++ ;
            *buf_ptr++ = (unsigned char) *p4++ ;
          }
          buf_ptr = buf_ptr + skip;
        }
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
        mcupos = MCUbuf + (i*nhMCU+j)*256;

        p1 = mcupos;
        p2 = mcupos+64;
        p3 = p2 + 64;
        p4 = p3 + 64;

        c1 = outbuf + i*block_row_skip + j*8;
        c2 = c1 + width * height;
        c3 = c2 + width * height;
        c4 = c3 + width * height;
        for (k = 8; k > 0; k--) {
          for (t = 8; t > 0; t--) {
            *c1++ = (unsigned char) *p1++ ;
            *c2++ = (unsigned char) *p2++ ;
            *c3++ = (unsigned char) *p3++ ;
            *c4++ = (unsigned char) *p4++ ;
          }
          c1 = c1 + skip;
          c2 = c2 + skip;
          c3 = c3 + skip;
          c4 = c4 + skip;
        }
      } /* end of j */
    } /* end of i */
  } /* end of else loop */
}

void Write_Scan_MCUs_11(
unsigned char *outbuf, /* image out */
int *MCUbuf, /* sequence of MCUs */
int width,   /* image width */
int height,  /* image height */
int interleave  /* Interleave flag */
)
/* 
 * Routine to generate raster scan data from macroblocks 
   for NON-subsampled data with two components (1:1)
   Not very practical, but included any how!!
 * MCUbuf: a pointer to the MCUs
 * if  interleaved=1, data output is interleaved (eg. MCYK, MCYK,...)
 * else it is not.
 *
*/
{
  unsigned char *buf_ptr;
  int *p1, *p2, i, j, k, t, skip, block_row_skip;
  int nvMCU, nhMCU;         /* number of FULL MCU vert. and hori. */
  unsigned char *c1, *c2;
  int *mcupos;

  nvMCU = (height)/8;
  nhMCU = (width)/8;

  if(interleave == 1) {
    skip = (2*width) - 16;
    block_row_skip = 2*width*8;
    for (i = 0; i < nvMCU; i++) {
      for (j = 0; j < nhMCU; j++) {
        mcupos = MCUbuf + (i*nhMCU+j)*128;

        p1 = mcupos;
        p2 = mcupos+64;

        buf_ptr = outbuf + i*block_row_skip + j*16;
        for (k = 8; k > 0; k--) {
          for (t = 8; t > 0; t--) {
            *buf_ptr++ = (unsigned char) *p1++ ;
            *buf_ptr++ = (unsigned char) *p2++ ;
          }
          buf_ptr = buf_ptr + skip;
        }
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
        mcupos = MCUbuf + (i*nhMCU+j)*128;

        p1 = mcupos;
        p2 = mcupos+64;

        c1 = outbuf + i*block_row_skip + j*8;
        c2 = c1 + width * height;
        for (k = 8; k > 0; k--) {
          for (t = 8; t > 0; t--) {
            *c1++ = (unsigned char) *p1++ ;
            *c2++ = (unsigned char) *p2++ ;
          }
          c1 = c1 + skip;
          c2 = c2 + skip;
        }
      } /* end of j */
    } /* end of i */
  } /* end of else loop */
}
