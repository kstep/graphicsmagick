/* -*-C-*-
******************************************************************************
*
* File:         process.c
* RCS:          $Header$
* Description:  Default subsampling/YCbCr routines for the HP JPEG Encoder.
* Author:       Kirt Winter
* Created:      Fri Mar  1 09:04:22 1995
* Initial Source Release:     Thursday, March 7 1996
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
#include "jpegconf.h"
#include "fpxmem.h"

#include "ejpeg.h"      /* CHG_JPEG_MEM_FIX - added this to get error code */

/*
** This routine strips color information from an opponent color stream.  Upon completion of
** its assigned duty, the destination buffer will be filled with LLLLabLLLLabLLLLab,
** assuming of course that it was originally LabLabLabLabLab.  Alpha channel is
** supported (as the last channel in the set), but is not subsampled.
**
*/
int SubSample411(unsigned char*src, unsigned char*dst, int tileSize, int bpp)
{
  int x, y, blocks, acc, lineOffset;
  unsigned char *dstPtr = dst;
  unsigned char *p1, *p2, *p3, *p4;

  blocks = tileSize / 2;
  lineOffset = tileSize * bpp;
  for(x = 0; x < blocks; x++)
  {
    for(y = 0; y < blocks; y++)
    {
      p1 = src;
      p2 = p1 + bpp;
      p3 = p1 + lineOffset;
      p4 = p3 + bpp;

      *dstPtr++ = *p1++;
      *dstPtr++ = *p2++;
      *dstPtr++ = *p3++;
      *dstPtr++ = *p4++;
      /* p1 - p4 now point to first chroma sample
          ** We take the average chroma of the 4 points in the quadrant
          ** and place that in the destination buffer. 
      */
      acc = *p1++ + *p2++ + *p3++ + *p4++;
      *dstPtr++ = (unsigned char) ((acc + 2) >> 2);

      /* p1 - p4 now point to second chroma sample
         ** now, we repeat the above process for second chroma
      */
      acc = *p1++ + *p2++ + *p3++ + *p4++;
      *dstPtr++ = (unsigned char) ((acc + 2) >> 2);

      /* Alpha channel, if it exists, is never subsampled */
      if (bpp == 4)
      {
        *dstPtr++ = *p1++;
        *dstPtr++ = *p2++;
        *dstPtr++ = *p3++;
        *dstPtr++ = *p4++;
      }

      src += 2*bpp;   /* skip over one triplet (quadlet?) and point to
                                                 the next quadrant */
    }
    src += lineOffset;
  }
  return(0);
}

int SubSample422(unsigned char *src, unsigned char *dst, int tileSize, int bpp)
{
  int x, y, blocks, acc, lineOffset;
  unsigned char *dstPtr = dst;
  unsigned char *p1, *p2;

  blocks = tileSize / 2;
  lineOffset = tileSize * bpp;
  for(x = 0; x < tileSize; x++)
  {
    for(y = 0; y < blocks; y++)
    {
      p1 = src;
      p2 = p1 + bpp;

      *dstPtr++ = *p1++;
      *dstPtr++ = *p2++;

      /* p1,p2 now point to first chroma sample
          ** here we take the average chroma of the two points
          ** and place that in the destination buffer 
      */
      acc = *p1++ + *p2++;
      *dstPtr++ = (unsigned char) ((acc + 1) >> 1);

      /* p1,p4 now point to second chroma sample
          ** so, we repeat the above process for second chroma
      */
      acc = *p1++ + *p2++;
      *dstPtr++ = (unsigned char) ((acc + 1) >> 1);

      /* If an alpha channel, it is never subsampled.*/
      if(bpp == 4)
      {
        *dstPtr++ = *p1++;
        *dstPtr++ = *p2++;
      }

      src += 2*bpp;   /* skip over one triplet (quadlet?) and point to
                                                 the next quadrant. */
    }
  }
  return(0);
}

/*
** This routine does an RGB to YCbCr conversion.  After complettion, the
** destination buffer will be filled with YCbCrYCbCrYCbCrYCbCrYCbCr,
** assuming of course that it was originally RGBRGBRGBRGBRGB.
** The routine also handles RGBa data, and in that situation, you'll
** get YCbCraYCbCraYCbCra...
**   
*/
int RGBtoYCrCb(unsigned char *src, unsigned char *dst, int tilerow, int bpp)
{
  int x;
  int tilesize = tilerow * tilerow * bpp;
  unsigned char inpR,inpG,inpB;
  long yo, cr, cb;
  long A1, A2, A3, A4;
  float F1, F2;

  /*
  ** the rotation routine is out of section 2.4.4.2
  ** of Pennebaker/Mitchell book.
  ** r,g,b values of 0-255 are mapped to 0-1 range, and then 
  ** all values are left shifted by 26 bits.
  */
  #if 0
  /* Pennebaker book's numbers: */
  A1 = 78952;
  A2 = 157903;
  A3 = 26317;
  A4 = 33554432;
  F1 = (float)1.6;
  #endif

  #if 1
  /* IJG's numbers: */
  A1 = 78381;
  A2 = 153879;
  A3 = 29884;
  A4 = 33554432;
  F1 = (float)1.402;
  F2 = (float)1.772;
  #endif

  if (bpp==3) {
    for(x = 0; x < tilesize; x+=bpp)
    {
      yo = A1*(long)(src[x]) + A2*(long)(src[x+1]) + A3*(long)(src[x+2]);

      /* old (Pennebaker book's) way:
      cb = (long)((((src[x+2])<<18) - yo) >> 1) + A4;
      cr = (long)((((src[x])<<18) - yo) / F1) + A4;
      */
      cb = (long)((((src[x+2])<<18) - yo) / F2) + A4;
      cr = (long)((((src[x])<<18) - yo) / F1) + A4;

      dst[x] = (unsigned char)(yo >> 18);
      dst[x+1] = (unsigned char)(cb >> 18);
      dst[x+2] = (unsigned char)(cr >> 18);
    }
  } else { /* bpp = 4 */
    for(x = 0; x < tilesize; x+=bpp)
    {
      inpR= (unsigned char) (255-src[x]);
      inpG= (unsigned char) (255-src[x+1]);
      inpB= (unsigned char) (255-src[x+2]);
      yo = A1*(long)(inpR) + A2*(long)(inpG) + A3*(long)(inpB);

      /* old (Pennebaker book's) way:
      cb = (long)((((inpB)<<18) - yo) >> 1) + A4;
      cr = (long)((((inpR)<<18) - yo) / F1) + A4;
      */
      cb = (long)((((inpB)<<18) - yo) / F2) + A4;
      cr = (long)((((inpR)<<18) - yo) / F1) + A4;
      dst[x] = (unsigned char)(yo >> 18);
      dst[x+1] = (unsigned char)(cb >> 18);
      dst[x+2] = (unsigned char)(cr >> 18);
      dst[x+3] = src[x+3];
    }
  }
  return(0);

} /* RGBtoYCrCb() */


/*
** This routine does an RGB to YCbCr conversion, then strips color information.  After
** completion, the destination buffer will be filled with YYYYCbCrYYYYCbCrYYYYCbCr,
** assuming of course that it was originally RGBRGBRGBRGBRGB.  Note that the routine does
** support conversion in place (i.e. src and dst can be the same buffer).
*/
int RGBtoYCrCb_SubSample411(unsigned char *src, unsigned char *dst, 
int tilerow, int bpp)
{
  int x, y, z, blocks, lineOffset;
  unsigned char *dstPtr = dst;
  unsigned char *p[4], *q[4];
  unsigned char inpR,inpG,inpB;
  long yo[4], cr, cb;
  long A1, A2, A3, A4;
  float F1, F2;

  blocks = tilerow / 2;
  lineOffset = tilerow * bpp;

  #if 0
  /* Pennebaker book's numbers */
  A1 = 78952;
  A2 = 157903;
  A3 = 26317;
  A4 = 33554432;
  F1 = 1.6;
  F2 = 2.0; /* this can be a right shift */
  #endif

  #if 1
  /* IJG's numbers: */
  A1 = 78381;
  A2 = 153879;
  A3 = 29884;
  A4 = 33554432;
  F1 = (float)1.402;
  F2 = (float)1.772;
  #endif

  if (bpp == 3)
  { /* 3 channel */
    for(x = 0; x < blocks; x++)
    {
      for(y = 0; y < blocks; y++)
      {
        p[0] = src;
        p[1] = p[0] + bpp;
        p[2] = p[0] + lineOffset;
        p[3] = p[2] + bpp;
        cb = 0;
        cr = 0;
        for(z = 0; z < 4; z++)
        {
          yo[z] = A1*(long)*(p[z]) + A2*(long)*(p[z]+1) + A3*(long)*(p[z]+2);

          /* old (pink's book) way:
          cb += ((((*(p[z]+2)<<18) - yo[z]) >> 1) + A4) >> 2;
          cr += ((long)(((*(p[z])<<18) - yo[z]) / F1) + A4) >> 2;
          */
          cb += ((long)((((*(p[z]+2)<<18) - yo[z]) / F2) + A4) >> 2);
          cr += ((long)((((*(p[z])<<18) - yo[z]) / F1) + A4) >> 2);
          
          *dstPtr++ = (unsigned char)(yo[z] >> 18);
        }

        /* now it's time to write out a "u" and a "v" */
        *dstPtr++ = (unsigned char)(cb >> 18);
        *dstPtr++ = (unsigned char)(cr >> 18);

        src += bpp*2; /* skip over one triplet and point to the next quadrant */
      } /* for y */

      src += lineOffset;
    } /* for x */

  }
  else if (bpp == 4)
  { /* 4 channel */
    for(x = 0; x < blocks; x++)
    {
      for(y = 0; y < blocks; y++)
      {
        p[0] = src;
        p[1] = p[0] + bpp;
        p[2] = p[0] + lineOffset;
        p[3] = p[2] + bpp;

        q[0]=p[0]+3; /* used for alpha channel */
        q[1]=p[1]+3;
        q[2]=p[2]+3;
        q[3]=p[3]+3;
        cb = 0;
        cr = 0;
        for(z = 0; z < 4; z++)
        {
          inpR = (unsigned char) (255-*p[z]);
          inpG = (unsigned char) (255-*(p[z]+1));
          inpB = (unsigned char) (255-*(p[z]+2));
          yo[z] = A1*(long)inpR + A2*(long)inpG + A3*(long)inpB;

          /* old (Pennebaker book's) way:
          cb += ((((inpB<<18) - yo[z]) >> 1) + A4) >> 2;
          cr += ((long)(((inpR<<18) - yo[z]) / F1) + A4) >> 2;
          */
          cb += ((long)((((inpB<<18) - yo[z]) / F2) + A4) >> 2);
          cr += ((long)((((inpR<<18) - yo[z]) / F1) + A4) >> 2);
          
          *dstPtr++ = (unsigned char)(yo[z] >> 18);
        }

        /* now it's time to write out a "u" and a "v" */
        *dstPtr++ = (unsigned char)(cb >> 18);
        *dstPtr++ = (unsigned char)(cr >> 18);

        /* write out the Alpha channel */
        *dstPtr++ = *q[0];
        *dstPtr++ = *q[1];
        *dstPtr++ = *q[2];
        *dstPtr++ = *q[3];

        src += bpp*2; /* skip over one triplet and point to the next quadrant */
      }
      src += lineOffset;
    }
  }
  return(0);

} /* RGBtoYCrCb_SubSample411() */


/*
** This routine does an RGB to YCbCr conversion, then strips color information.
** This is not done particularly efficiently, but it's important that it get done
** correctly and on time.  So, rather than write an integrated routine like the 411
** subsampling, the individual pieces are called, simplifying coding and testing.
*/
int RGBtoYCrCb_SubSample422(unsigned char*src, unsigned char*dst, int tileSize, int bpp)
{
  unsigned char *dst0;
  int error;

  dst0 = (unsigned char *)FPX_malloc((size_t)(tileSize * tileSize * bpp));
  if(dst0 == NULL)            /* CHG_JPEG_MEM_FIX - added check */
    return EJPEG_ERROR_MEM;

  RGBtoYCrCb(src, dst0, tileSize, bpp);
  error = SubSample422(dst0, dst, tileSize, bpp);

  FPX_free(dst0);

  return error;

} /* RGBtoYCrCb_SubSample422() */
