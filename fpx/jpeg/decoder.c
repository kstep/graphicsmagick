/* -*-C-*-
******************************************************************************
*
* File:         decoder.c
* RCS:          $Header$
* Description:  main module of JPEG decoder library.
* Author:       Peyma Oskoui
* Created:      Fri Dec  4 11:04:22 1992
* Initial Source Release:     Mon Jan 22 1996
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
#include <string.h>
#include "jpegconf.h"

#include "decoder.h"
#include "fpxmem.h"

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

/* static prototypes: */

static int dJPEG_Upsample411(unsigned char *dataBuffer, 
void *decoder);
static int dJPEG_Upsample4114(unsigned char *dataBuffer, 
void *decoder);
static int dJPEG_Upsample422(unsigned char *dataBuffer, size_t oneTile_inBytes,
void *decoder);
static int dJPEG_Upsample4224(unsigned char *dataBuffer, size_t oneTile_inBytes,
void *decoder);
static int dJPEG_Upsample411AndConvert(unsigned char *dataBuffer, 
void *decoder);
static int dJPEG_Upsample4114AndConvert(unsigned char *dataBuffer, 
void *decoder);
static int dJPEG_Upsample422AndConvert(unsigned char *dataBuffer, 
size_t oneTile_inBytes, 
void *decoder);
static int dJPEG_Upsample4224AndConvert(unsigned char *dataBuffer, 
size_t oneTile_inBytes, 
void *decoder);
static int dJPEG_YCbCrToRGB(unsigned char *dataBuffer,  size_t oneTile_inBytes,
void *decoder);


int dJPEG_DecoderInit(void **decoder)
{
  DECODER_STRUCT *decodePtr = NULL;
  int i;

  if ( (*decoder = (DECODER_STRUCT *)FPX_malloc(sizeof(DECODER_STRUCT))) == NULL)
    {
      return DJPEG_ERR_ALLOC_DECODER;
    }

  decodePtr = (DECODER_STRUCT *)*decoder;
  /* initialize */

  decodePtr->num_channels = 3; /* default...use dJPEG_SetTileSize() to 
                                  change this. */
  decodePtr->hSize = 64; /* default */
  decodePtr->vSize = 64;

  decodePtr->InternalUpsample = TRUE;
  decodePtr->InternalColor = TRUE;
  decodePtr->horiz_subsample = 1;
  decodePtr->vert_subsample = 1;
  decodePtr->internal_colorconvert = 1;

  for (i=0; i < 4 ;i++) {
    decodePtr->huffman_table_dc[i]= NULL;
    decodePtr->huffman_table_ac[i]= NULL;
    decodePtr->quant_table[i] = NULL;
    decodePtr->mcu_buf[i] = NULL;
  }
  decodePtr->numb_quant_tables = 0;
  decodePtr->numb_huffman_pair_tables = 0;

  decodePtr->frame = NULL;
  decodePtr->row_bufs = NULL;
  for (i=0; i < 16 ;i++) {
    decodePtr->irow_bufs[i]=NULL;
  }

  return DJPEG_NO_ERROR;

} /* dJPEG_DecoderInit() */


void dJPEG_DisableUpsample(void *decoder)
{
  DECODER_STRUCT *decodePtr = (DECODER_STRUCT *)decoder;

  decodePtr->InternalUpsample = FALSE;

} /* dJPEG_DisableUpsample() */

void dJPEG_DisableColorConvert(void *decoder)
{
  DECODER_STRUCT *decodePtr = (DECODER_STRUCT *)decoder;

  decodePtr->InternalColor = FALSE;

} /* dJPEG_DisableColorConvert() */

void dJPEG_EnableUpsample(void *decoder)
{
  DECODER_STRUCT *decodePtr = (DECODER_STRUCT *)decoder;

  decodePtr->InternalUpsample = TRUE;

} /* dJPEG_EnableUpsample() */

void dJPEG_EnableColorConvert(void *decoder)
{
  DECODER_STRUCT *decodePtr = (DECODER_STRUCT *)decoder;

  decodePtr->InternalColor = TRUE;

} /* dJPEG_EnableColorConvert() */

int dJPEG_SetTileSize( void *decoder, 
                       int hSize,  /* width of a tile in pixels */
                       int vSize,  /* height of a tile in pixels */
                       int num_channels /* number of channels (or bytes per pixel) in use */
                       )
{
  DECODER_STRUCT *decodePtr = (DECODER_STRUCT *)decoder;

  if (decodePtr == NULL)
    return FALSE;

  if ((num_channels > 4 ) || (num_channels < 1))
    return DJPEG_ERR_BAD_CHANNELS;

  decodePtr->hSize = hSize;
  decodePtr->vSize = vSize;
  decodePtr->num_channels = num_channels;

  return DJPEG_NO_ERROR;

} /* dJPEG_SetTileSize() */

int dJPEG_CopyJpegSubtype(void *decoder, unsigned long JpegSubtype)
{
  DECODER_STRUCT *decodePtr = (DECODER_STRUCT *)decoder;
  JPEGSUBTYPE  subtypeInfo;

  subtypeInfo.interleave_type = 
    (unsigned char)(0x000000FF & JpegSubtype);  /* byte 0 */

  subtypeInfo.chroma_subsample =  
    (unsigned char)((0x0000FF00 & JpegSubtype) >> 8); /* byte 1 */

  subtypeInfo.internal_colorconvert = 
    (unsigned char)((0x00FF0000 & JpegSubtype) >> 16); /* byte 2 */

  subtypeInfo.jpeg_tblselector = 
    (unsigned char)((0xFF000000 & JpegSubtype) >> 24); /* byte 3 */

  subtypeInfo.horiz_subsample = 
    (unsigned char) LEFT_NIBBLE(subtypeInfo.chroma_subsample);

  subtypeInfo.vert_subsample = 
    (unsigned char) RIGHT_NIBBLE(subtypeInfo.chroma_subsample);

  /* some value checking: */
  if ( subtypeInfo.interleave_type != 0 && subtypeInfo.interleave_type != 1)
    return DJPEG_ERR_BAD_INTERLEAVE;

  if (subtypeInfo.horiz_subsample == 4 || subtypeInfo.vert_subsample == 4)
    return DJPEG_ERR_BAD_SUBSAMPLE;

  /* we can only handle 4:1:1 (0x22) and 4:2:2 (0x21) and 1:1:1 (0x11) */
  if (subtypeInfo.horiz_subsample > 2 || subtypeInfo.vert_subsample  > 2)
    return DJPEG_ERR_BAD_SUBSAMPLE;

  if (subtypeInfo.internal_colorconvert != 0 && 
      subtypeInfo.internal_colorconvert != 1)
    return DJPEG_ERR_BAD_COLOR_CONVERT;

  /* copy over to decoder structure of jpeg after casting it properly */
  decodePtr->interleave_type  =   subtypeInfo.interleave_type;
  decodePtr->internal_colorconvert =  subtypeInfo.internal_colorconvert;
  decodePtr->horiz_subsample  =   subtypeInfo.horiz_subsample;
  decodePtr->vert_subsample   = subtypeInfo.vert_subsample;
  decodePtr->chroma_subsample =   subtypeInfo.chroma_subsample;
  decodePtr->jpeg_tblselector =   subtypeInfo.jpeg_tblselector;

  return DJPEG_NO_ERROR;

} /* dJPEG_CopyJpegSubtype() */

void dJPEG_DecoderFree(void *decoder, int freeStructure)
{
  /* frees all buffers allocated in decoder structure. If the second arg is 1,
     it at the end also frees the structure itself.
     For entropy_nad_header case, where the huffman/quant tables are 
     reallocated for each tile, dJPEG_DecoderFree is called with second
     arg set to 0 (FALSE). 
     For Entropy_only case, where dJPEG_DecoderFree() is called only once
     after all tiles are processed, the second arg is 1.
  */

  DECODER_STRUCT *decoderPtr = (DECODER_STRUCT *)decoder;
  int i, tc;

  for (i=0; i < decoderPtr->numb_huffman_pair_tables ;i++) {
    if (decoderPtr->huffman_table_dc[i] != NULL) {
      for (tc = 0; tc < 256; tc++) {
        if ((decoderPtr->huffman_table_dc[i]->huffelem[tc].hufftree)!=NULL) {
          FPX_free(decoderPtr->huffman_table_dc[i]->huffelem[tc].hufftree);
          decoderPtr->huffman_table_dc[i]->huffelem[tc].hufftree=NULL;
        }
      }
      FPX_free(decoderPtr->huffman_table_dc[i]);
      decoderPtr->huffman_table_dc[i] = NULL;
    }
    if (decoderPtr->huffman_table_ac[i] != NULL) {
      for (tc = 0; tc < 256; tc++) {
        if ((decoderPtr->huffman_table_ac[i]->huffelem[tc].hufftree)!=NULL) {
          FPX_free(decoderPtr->huffman_table_ac[i]->huffelem[tc].hufftree);
          decoderPtr->huffman_table_ac[i]->huffelem[tc].hufftree=NULL;
        }
      }
      FPX_free(decoderPtr->huffman_table_ac[i]);
      decoderPtr->huffman_table_ac[i] = NULL;
    }
  }

  for (i=0; i < decoderPtr->numb_quant_tables ;i++) {
    if (decoderPtr->quant_table[i] != NULL) {
      FPX_free(decoderPtr->quant_table[i]);
      decoderPtr->quant_table[i] = NULL;
    }
  }
  if (decoderPtr->frame != NULL)
    if (decoderPtr->frame->comps != NULL) {
      FPX_free(decoderPtr->frame->comps);
      decoderPtr->frame->comps = NULL;
    }

  if (decoderPtr->frame != NULL)  {
    FPX_free(decoderPtr->frame);
    decoderPtr->frame = NULL;
  }

  for (i=0; i < 4 ;i++) {
    if (decoderPtr->mcu_buf[i] != NULL) {
      FPX_free(decoderPtr->mcu_buf[i]);
      decoderPtr->mcu_buf[i] = NULL;
    }
  }
  for (i=0; i < 16 ;i++) {
    if (decoderPtr->irow_bufs[i] != NULL) {
      FPX_free(decoderPtr->irow_bufs[i]);
      decoderPtr->irow_bufs[i] = NULL;
    }
  }
  if (decoderPtr->row_bufs != NULL)  {
    FPX_free(decoderPtr->row_bufs);
    decoderPtr->row_bufs = NULL;
  }

  if (freeStructure && decoderPtr != NULL)
    FPX_free(decoderPtr);
}

int dJPEG_UpsampleAndConvert(DECODER_STRUCT *decoder, 
                             unsigned char *dataBuffer,  
                             size_t oneTile_inBytes)
{
  int format = WRONG_SAMPLE;  /* format of the subsampling */
  int num_channels;   /* number of color channels */

  /*
  ** change from RRRRGB format to RGBRGBRGBRGB format after 
  ** decompressing. 
  ** format is the format of data after decompression (say RRRRGB 
  ** vs RGRBRGRB vs what not)
  ** dataBuffer holds the incoming data (decompressed tile) at 
  ** input and the Upsampled tile at output.
  ** oneTile_inBytes is the size of the tile of data.
  **
  ** Note: we use an decimal addition method to keep track of
  **       which operations to do: color conversion is 1000, format
  **       is initialized to 0, and is added by 411 or 422 or 111 
  **       (depending on upsample type) and then added by 1000 if
  **       YCbCr rotation is needed as well...So the switch statement
  **       to decide which operations to perform used this number.
  */

  num_channels = decoder->num_channels;

  if (num_channels == 3 && 
      decoder->horiz_subsample == 2 && decoder->vert_subsample == 2)
    format += FOUR_ONE_ONE;
  else if (num_channels == 4 &&
           decoder->horiz_subsample == 2 && decoder->vert_subsample == 2)
    format += FOUR_ONE_ONE_FOUR;
  else if (num_channels == 3 &&
           decoder->horiz_subsample == 2 && decoder->vert_subsample == 1)
    format += FOUR_TWO_TWO;
  else if (num_channels == 4 &&
           decoder->horiz_subsample == 2 && decoder->vert_subsample == 1)
    format += FOUR_TWO_TWO_FOUR;
  else if (decoder->horiz_subsample == 1 && decoder->vert_subsample == 1) {
    if (num_channels == 3)
      format += ONE_ONE_ONE;
    else if (num_channels == 4)
      format += ONE_ONE_ONE_ONE;
  }

  /* if internal upsampling is turned off, reset the format to zero */
  if (!decoder->InternalUpsample)
    format = WRONG_SAMPLE;

  if (decoder->InternalColor && (decoder->internal_colorconvert == 1))
    format += COLOR_CONVERT;

  switch (format)
    {
    case FOUR_ONE_ONE :
      if (!dJPEG_Upsample411(dataBuffer, 
                             (void *)decoder))
        return DJPEG_ERR_BAD_411;
      break;
    case FOUR_ONE_ONE_FOUR :
      if (!dJPEG_Upsample4114(dataBuffer, 
                              (void *)decoder))
        return DJPEG_ERR_BAD_4114;
      break;
    case ONE_ONE_ONE :
      /* no need for upsampling or color rotation */
      break;
    case ONE_ONE_ONE_ONE :
      /* no need for upsampling or color rotation */
      break;
    case FOUR_TWO_TWO :
      if (!dJPEG_Upsample422(dataBuffer, oneTile_inBytes,
                             (void *)decoder))
        return DJPEG_ERR_BAD_422;
      break;
    case FOUR_TWO_TWO_FOUR :
      if (!dJPEG_Upsample4224(dataBuffer, oneTile_inBytes,
                              (void *)decoder))
        return DJPEG_ERR_BAD_4224;
      break;
    case COLOR_AND_FOUR_ONE_ONE :
      if (!dJPEG_Upsample411AndConvert(dataBuffer, 
                                       (void *)decoder))
        return DJPEG_ERR_BAD_411CONVERT;
      break;
    case COLOR_AND_FOUR_ONE_ONE_FOUR :
      if (!dJPEG_Upsample4114AndConvert(dataBuffer, 
                                        (void *)decoder))
        return DJPEG_ERR_BAD_4114CONVERT;
      break;
    case COLOR_AND_FOUR_TWO_TWO_FOUR :
      if (!dJPEG_Upsample4224AndConvert(dataBuffer, oneTile_inBytes,
                                        (void *)decoder))
        return DJPEG_ERR_BAD_4224CONVERT;
      break;
    case COLOR_AND_ONE_ONE_ONE  :
      if (!dJPEG_YCbCrToRGB(dataBuffer, oneTile_inBytes,
                            (void *)decoder))
        return DJPEG_ERR_BAD_111CONVERT;
      break;
    case COLOR_AND_ONE_ONE_ONE_ONE  :
    case COLOR_CONVERT :
      if (!dJPEG_YCbCrToRGB(dataBuffer, oneTile_inBytes,
                            (void *)decoder))
        return DJPEG_ERR_BAD_1111CONVERT;
      break;
    case COLOR_AND_FOUR_TWO_TWO :
      if (!dJPEG_Upsample422AndConvert(dataBuffer, oneTile_inBytes,
                                       (void *)decoder))
        return DJPEG_ERR_BAD_422CONVERT;
      break;

    default:
      return DJPEG_ERR_BAD_SWITCH;
      break;

    }  /* switch on format of the decoded data */

  return DJPEG_NO_ERROR;

} /* dJPEG_UpsampleAndConvert() */

static int dJPEG_Upsample411(unsigned char *dataBuffer, 
                             void *decoder)
{
  /*
  ** Y1 Y2 Y3 Y4 Cr Cb  format becomes two rows of 
  ** Y1 Cr Cb Y2 Cr Cb
  ** Y3 Cr Cb Y4 Cr Cb
  */

  DECODER_STRUCT *decodePtr = (DECODER_STRUCT *)decoder;
  int num_channels = decodePtr->num_channels;
  unsigned char  *templine1, *templine2, *srcPtr;
  int i,j,irow;
  long onerowinBytes = 64 * num_channels;

  srcPtr = dataBuffer;

  /* Upsample in place: start from end and work up to start of buffer */
  for (irow = 32, j = 64; irow > 0; j-=2, irow--)
    {
      unsigned char L1, L2, L3, L4, u, v;

      templine2 = dataBuffer + (j * onerowinBytes) - 1;
      templine1 = templine2 - onerowinBytes;

      srcPtr = dataBuffer + (irow * onerowinBytes) - 1;
      for (i=0; i< onerowinBytes; i+=6)
        {
          v = *srcPtr--;
          u = *srcPtr--;
          L4 = *srcPtr--;
          L3 = *srcPtr--;
          L2 = *srcPtr--;
          L1 = *srcPtr--;

          *templine1-- = v;
          *templine1-- = u;
          *templine1-- = L2;
          *templine1-- = v;
          *templine1-- = u;
          *templine1-- = L1;

          *templine2-- = v;
          *templine2-- = u;
          *templine2-- = L4;
          *templine2-- = v;
          *templine2-- = u;
          *templine2-- = L3;
        }
    }

  return TRUE;

} /* dJPEG_Upsample411() */


static int dJPEG_Upsample4114(unsigned char *dataBuffer, 
                              void *decoder)
{
  /*
  ** Y1 Y2 Y3 Y4 Cr Cb A A A A  format becomes two rows of 
  ** Y1 Cr Cb A Y2 Cr Cb A
  ** Y3 Cr Cb A Y4 Cr Cb A
  */
  unsigned char  tempBuffer[16384]; /* size of one tile */
  unsigned char *templine1, *templine2, *srcPtr;
  int i,j,irow;
  long onerowinSSbytes = 64/2 * 10;
  long onedstrowinBytes;
  DECODER_STRUCT *decodePtr = (DECODER_STRUCT *)decoder;
  int num_channels = decodePtr->num_channels;
  unsigned long dstSize;
  unsigned char L1, L2, L3, L4, u,v,A1,A2,A3,A4;

  onedstrowinBytes = 64 * num_channels;
  srcPtr = dataBuffer;
  dstSize = onedstrowinBytes * 64;

  for (irow = 0, j = 0; irow < 64; j++, irow+=2)
    {
      templine1 = tempBuffer + (j * 2* onedstrowinBytes);
      templine2 = tempBuffer + ((j*2 +1)* onedstrowinBytes);
      srcPtr = dataBuffer + (j * onerowinSSbytes);

      for (i=0; i< onedstrowinBytes; i+=8)
        {
          L1 = *srcPtr++;
          L2 = *srcPtr++;
          L3 = *srcPtr++;
          L4 = *srcPtr++;
          u = *srcPtr++;
          v = *srcPtr++;
          A1 = *srcPtr++;
          A2 = *srcPtr++;
          A3 = *srcPtr++;
          A4 = *srcPtr++;

          *templine1++ = L1;    /* L1 */
          *templine1++ = u;
          *templine1++ = v;
          *templine1++ = A1;

          *templine1++ = L2;    /* L2 */
          *templine1++ = u;
          *templine1++ = v;
          *templine1++ = A2;

          *templine2++ = L3;   /* L3 */
          *templine2++ = u;
          *templine2++ = v;
          *templine2++ = A3;

          *templine2++ = L4;   /* L4 */
          *templine2++ = u;
          *templine2++ = v;
          *templine2++ = A4;

        }

    } /* for all rows of data in dataBuffer */

  /* copy back to output buffer */
  memcpy (dataBuffer, tempBuffer, dstSize);

  return TRUE;

} /* dJPEG_Upsample4114() */


static int dJPEG_Upsample422(unsigned char *dataBuffer, size_t oneTile_inBytes,
                             void *decoder)
{
  /*
  ** Y1Y2u1v1 Y3Y4u2v2 ... Y63Y64u32v32
  ** only sampled in one direction...horizontal direction 
  ** (make sure the order is YYuv, 
  ** 
  */

  DECODER_STRUCT *decodePtr = (DECODER_STRUCT *)decoder;
  int num_channels = decodePtr->num_channels;
  unsigned char *srcptr, u, v, Y1, Y2;
  unsigned long size, i, jump;
  unsigned char tempBuffer[16384];
  unsigned char *destptr;

  srcptr = dataBuffer;
  destptr = tempBuffer;
  size = 64 * 64 * num_channels;
  jump = 2 * num_channels;

  for (i=0; i<size; i+=jump)
    {
      Y1 = *srcptr++;
      Y2 = *srcptr++;
      u = *srcptr++;
      v = *srcptr++;
      *destptr++ = Y1;
      *destptr++ = u;
      *destptr++ = v;
      *destptr++ = Y2;
      *destptr++ = u;
      *destptr++ = v;
    }

  memcpy(dataBuffer, tempBuffer,oneTile_inBytes);

  return TRUE;

} /* dJPEG_Upsample422() */


static int dJPEG_Upsample4224(unsigned char *dataBuffer, size_t oneTile_inBytes,
                              void *decoder)
{
  /*
  ** Y1Y2u1v1A1A2 Y3Y4u2v2A1A2 ... Y63Y64u32v32A63A64
  ** only sampled in one direction...horizontal direction 
  ** (make sure the order is YYuv, 
  ** 
  */

  DECODER_STRUCT *decodePtr = (DECODER_STRUCT *)decoder;
  int num_channels = decodePtr->num_channels;
  unsigned char *srcptr, u, v, Y1, Y2, A1,A2;
  unsigned long size, i, jump;
  unsigned char tempBuffer[16384];
  unsigned char *destptr;

  srcptr = dataBuffer;
  destptr = tempBuffer;
  size = 64 * 64 * num_channels;
  jump = 2 * num_channels;

  for (i=0; i<size; i+=jump)
    {
      Y1 = *srcptr++;
      Y2 = *srcptr++;
      u = *srcptr++;
      v = *srcptr++;
      A1 = *srcptr++;
      A2 = *srcptr++;
      *destptr++ = Y1;
      *destptr++ = u;
      *destptr++ = v;
      *destptr++ = A1;
      *destptr++ = Y2;
      *destptr++ = u;
      *destptr++ = v;
      *destptr++ = A2;
    }

  memcpy(dataBuffer, tempBuffer,oneTile_inBytes);

  return TRUE;

} /* dJPEG_Upsample4224() */


static int dJPEG_Upsample411AndConvert(unsigned char *dataBuffer, 
                                       void *decoder)
{
  /*
  ** Y1 Y2 Y3 Y4 Cr Cb  format becomes two rows of 
  ** Y1 Cr Cb Y2 Cr Cb
  ** Y3 Cr Cb Y4 Cr Cb
  */

  DECODER_STRUCT *decodePtr = (DECODER_STRUCT *)decoder;
  int num_channels = decodePtr->num_channels;
  unsigned char  *templine1, *templine2, *srcPtr;
  int i,j,irow;
  long onerowinBytes = 64 * num_channels;

  srcPtr = dataBuffer;

  /* Upsample in place: start from end and work up to start of buffer */
  for (irow = 32, j = 64; irow > 0; j-=2, irow--)
    {
      unsigned char L1, L2, L3, L4, cb, cr;
      int ncr,ncb;
      int r1,g1,b1, r2, g2, b2, r3, g3, b3, r4, g4, b4;

      templine2 = dataBuffer + (j * onerowinBytes) - 1;
      templine1 = templine2 - onerowinBytes;

      srcPtr = dataBuffer + (irow * onerowinBytes) - 1;
      for (i=0; i< onerowinBytes; i+=6)
        {
          cr = *srcPtr--;
          cb = *srcPtr--;
          L4 = *srcPtr--;
          L3 = *srcPtr--;
          L2 = *srcPtr--;
          L1 = *srcPtr--;
#if 0
          /* pink book's method */
          u = (cb - 128) << 9;
          v = (cr - 128) * 410;
          ushift = u >> 8;
          vshift = v >> 8;

          r1 = CLIP((L1 + vshift));
          b1 = CLIP((L1 + ushift));
          g1 = CLIP(( ((L1<<8) - (77*r1) -(26*b1) ) * 427 ) >> 16);

          r2 = CLIP((L2 + vshift));
          b2 = CLIP((L2 + ushift));
          g2 = CLIP(( ((L2<<8) - (77*r2) -(26*b2) ) * 427 ) >> 16);

          r3 = CLIP((L3 + vshift));
          b3 = CLIP((L3 + ushift));
          g3 = CLIP(( ((L3<<8) - (77*r3) -(26*b3) ) * 427 ) >> 16);

          r4 = CLIP((L4 + vshift));
          b4 = CLIP((L4 + ushift));
          g4 = CLIP(( ((L4<<8) - (77*r4) -(26*b4) ) * 427 ) >> 16);
#endif
          ncr = cr;
          ncb = cb;

          /* IJG's method */
          r1 = CLIP((L1 + (((359*ncr) - 45645) >> 8)));
          b1 = CLIP((L1 + (((454*ncb) - 57838) >> 8)));
          g1 = CLIP(( (L1<<8) - (88 * ncb) - (183 * ncr) + 34542) >> 8);

          r2 = CLIP((L2 + (((359*ncr) - 45645) >> 8)));
          b2 = CLIP((L2 + (((454*ncb) - 57838) >> 8)));
          g2 = CLIP(( (L2<<8) - (88 * ncb) - (183 * ncr) + 34542) >> 8);

          r3 = CLIP((L3 + (((359*ncr) - 45645) >> 8)));
          b3 = CLIP((L3 + (((454*ncb) - 57838) >> 8)));
          g3 = CLIP(( (L3<<8) - (88 * ncb) - (183 * ncr) + 34542) >> 8);

          r4 = CLIP((L4 + (((359*ncr) - 45645) >> 8)));
          b4 = CLIP((L4 + (((454*ncb) - 57838) >> 8)));
          g4 = CLIP(( (L4<<8) - (88 * ncb) - (183 * ncr) + 34542) >> 8);

          *templine1-- = (unsigned char) b2;
          *templine1-- = (unsigned char) g2;
          *templine1-- = (unsigned char) r2;
          *templine1-- = (unsigned char) b1;
          *templine1-- = (unsigned char) g1;
          *templine1-- = (unsigned char) r1;

          *templine2-- = (unsigned char) b4;
          *templine2-- = (unsigned char) g4;
          *templine2-- = (unsigned char) r4;
          *templine2-- = (unsigned char) b3;
          *templine2-- = (unsigned char) g3;
          *templine2-- = (unsigned char) r3;
        }
    }
  return TRUE;

} /* dJPEG_Upsample411AndConvert() */


static int dJPEG_Upsample4114AndConvert(unsigned char *dataBuffer,
                                        void *decoder)
{
  /*
  ** Y1 Y2 Y3 Y4 Cr Cb A A A A  format becomes two rows of 
  ** R1 G B A R2 G B A
  ** R3 G B A R4 G B A
  */
  unsigned char  tempBuffer[16384];
  unsigned char *templine1, *templine2, *srcPtr;
  int i,j,irow;
  int r1,g1,b1, r2,g2,b2, r3,g3,b3, r4,g4,b4;
  long onerowinSSbytes = 320; /* 64/2 * 10 */
  long onedstrowinBytes;
  unsigned long dstSize;
  unsigned char Y1,Y2,Y3,Y4,cr,cb,A1,A2,A3,A4;

  onedstrowinBytes = 256; /* 64*num_channels */
  srcPtr = dataBuffer;
  dstSize = onedstrowinBytes * 64;

  for (irow = 0, j = 0; irow < 64; j++, irow+=2)
    {
      templine1 = tempBuffer + (j * 2* onedstrowinBytes);
      templine2 = tempBuffer + ((j*2 +1)* onedstrowinBytes);
      srcPtr = dataBuffer + (j * onerowinSSbytes);

      for (i=0; i< onedstrowinBytes; i+=8)
        {
          Y1 = *srcPtr++;
          Y2 = *srcPtr++;
          Y3 = *srcPtr++;
          Y4 = *srcPtr++;
          cb = *srcPtr++;
          cr = *srcPtr++;
          A1 = *srcPtr++;
          A2 = *srcPtr++;
          A3 = *srcPtr++;
          A4 = *srcPtr++;
#if 0
          /* pink book's method */
          u = (cb - 128) << 9;
          v = (cr - 128) * 410;
          ushift = u >> 8;
          vshift = v >> 8;

          r1 = CLIP((Y1 + vshift));
          b1 = CLIP((Y1 + ushift));
          g1 = CLIP(( ((Y1<<8) - (77*r1) -(26*b1) ) * 427 ) >> 16);

          r2 = CLIP((Y2 + vshift));
          b2 = CLIP((Y2 + ushift));
          g2 = CLIP(( ((Y2<<8) - (77*r2) -(26*b2) ) * 427 ) >> 16);

          r3 = CLIP((Y3 + vshift));
          b3 = CLIP((Y3 + ushift));
          g3 = CLIP(( ((Y3<<8) - (77*r3) -(26*b3) ) * 427 ) >> 16);

          r4 = CLIP((Y4 + vshift));
          b4 = CLIP((Y4 + ushift));
          g4 = CLIP(( ((Y4<<8) - (77*r4) -(26*b4) ) * 427 ) >> 16);
#endif
#if 1
          /* IJG's method */
          r1 = CLIP((Y1 + (((359*cr) - 45645) >> 8)));
          b1 = CLIP((Y1 + (((454*cb) - 57838) >> 8)));
          g1 = CLIP(( (Y1<<8) - (88 * cb) - (183 * cr) + 34542) >> 8);

          r2 = CLIP((Y2 + (((359*cr) - 45645) >> 8)));
          b2 = CLIP((Y2 + (((454*cb) - 57838) >> 8)));
          g2 = CLIP(( (Y2<<8) - (88 * cb) - (183 * cr) + 34542) >> 8);

          r3 = CLIP((Y3 + (((359*cr) - 45645) >> 8)));
          b3 = CLIP((Y3 + (((454*cb) - 57838) >> 8)));
          g3 = CLIP(( (Y3<<8) - (88 * cb) - (183 * cr) + 34542) >> 8);

          r4 = CLIP((Y4 + (((359*cr) - 45645) >> 8)));
          b4 = CLIP((Y4 + (((454*cb) - 57838) >> 8)));
          g4 = CLIP(( (Y4<<8) - (88 * cb) - (183 * cr) + 34542) >> 8);
#endif
          *templine1++ = (unsigned char) (255-r1);    /* R1 */
          *templine1++ = (unsigned char) (255-g1);
          *templine1++ = (unsigned char) (255-b1);
          *templine1++ = (unsigned char) A1;

          *templine1++ = (unsigned char) (255-r2);    /* R2 */
          *templine1++ = (unsigned char) (255-g2);
          *templine1++ = (unsigned char) (255-b2);
          *templine1++ = (unsigned char) A2;

          *templine2++ = (unsigned char) (255-r3);   /* R3 */
          *templine2++ = (unsigned char) (255-g3);
          *templine2++ = (unsigned char) (255-b3);
          *templine2++ = (unsigned char) A3;

          *templine2++ = (unsigned char) (255-r4);   /* R4 */
          *templine2++ = (unsigned char) (255-g4);
          *templine2++ = (unsigned char) (255-b4);
          *templine2++ = (unsigned char) A4;
        }

    } /* for all rows of data in dataBuffer */

  /* copy back to output buffer */
  memcpy (dataBuffer, tempBuffer, dstSize);

  return TRUE;

} /* dJPEG_Upsample4114AndConvert() */


static int dJPEG_Upsample422AndConvert(unsigned char *dataBuffer, 
                                       size_t oneTile_inBytes, 
                                       void *decoder)
{
  /*
  ** Y1Y2u1v1 Y3Y4u2v2 .... Y63Y64u32v32 
  ** only sampled in one direction...horizontal direction 
  ** (make sure the order is YYuv, and not uY, vY....how?!)
  ** 
  */

  DECODER_STRUCT *decodePtr = (DECODER_STRUCT *)decoder;
  int num_channels = decodePtr->num_channels;
  unsigned char *srcptr, cr, cb, Y1, Y2;
  int r1,g1,b1, r2,g2,b2;
  unsigned long size, i, jump;
  unsigned char tempBuffer[16384];
  unsigned char *destptr;

  srcptr = dataBuffer;
  destptr = tempBuffer;
  size = 64 * 64 * num_channels;
  jump = 2 * num_channels;

  for (i=0; i<size; i+=jump)
    {
      Y1 = *srcptr++;
      Y2 = *srcptr++;
      cb = *srcptr++;
      cr = *srcptr++;
#if 0
      /* pink book's method */
      u = (cb - 128) << 9;
      v = (cr - 128) * 410;
      ushift = u >> 8;
      vshift = v >> 8;

      r1 = CLIP((Y1 + vshift));
      b1 = CLIP((Y1 + ushift));
      g1 = CLIP(( ((Y1<<8) - (77*r1) -(26*b1) ) * 427 ) >> 16);

      r2 = CLIP((Y2 + vshift));
      b2 = CLIP((Y2 + ushift));
      g2 = CLIP(( ((Y2<<8) - (77*r2) -(26*b2) ) * 427 ) >> 16);
#endif
#if 1
      /* IJG's method */
      r1 = CLIP((Y1 + (((359*cr) - 45645) >> 8)));
      b1 = CLIP((Y1 + (((454*cb) - 57838) >> 8)));
      g1 = CLIP(((Y1<<8) - (88 * cb) - (183 * cr) + 34542) >> 8);

      r2 = CLIP((Y2 + (((359*cr) - 45645) >> 8)));
      b2 = CLIP((Y2 + (((454*cb) - 57838) >> 8)));
      g2 = CLIP(((Y2<<8) - (88 * cb) - (183 * cr) + 34542) >> 8);
#endif

      *destptr++ = (unsigned char) r1;
      *destptr++ = (unsigned char) g1;
      *destptr++ = (unsigned char) b1;
      *destptr++ = (unsigned char) r2;
      *destptr++ = (unsigned char) g2;
      *destptr++ = (unsigned char) b2;
    }

  memcpy(dataBuffer, tempBuffer,oneTile_inBytes);

  return TRUE;
} /* dJPEG_Upsample422AndConvert() */


static int dJPEG_Upsample4224AndConvert(unsigned char *dataBuffer, 
                                        size_t oneTile_inBytes, 
                                        void *decoder)
{
  /*
  ** Y1Y2u1v1A1A2 Y3Y4u2v2A3A4 .... Y63Y64u32v32A63A64 
  ** only sampled in one direction...horizontal direction 
  ** (make sure the order is YYuvAA)
  ** 
  */

  unsigned char *srcptr, cr, cb, Y1, Y2, A1, A2;
  int r1,g1,b1, r2,g2,b2;
  unsigned long size, i, jump;
  unsigned char tempBuffer[16384];
  unsigned char *destptr;

  srcptr = dataBuffer;
  destptr = tempBuffer;
  size = 64 * 64 * 4;  /* 64*64*num_channels */
  jump = 8;  /* 2 * num_channels */

  for (i=0; i<size; i+=jump)
    {
      Y1 = *srcptr++;
      Y2 = *srcptr++;
      cb = *srcptr++;
      cr = *srcptr++;
      A1 = *srcptr++;
      A2 = *srcptr++;
#if 0
      /* pink book's method */
      u = (cb - 128) << 9;
      v = (cr - 128) * 410;
      ushift = u >> 8;
      vshift = v >> 8;

      r1 = CLIP((Y1 + vshift));
      b1 = CLIP((Y1 + ushift));
      g1 = CLIP(( ((Y1<<8) - (77*r1) -(26*b1) ) * 427 ) >> 16);

      r2 = CLIP((Y2 + vshift));
      b2 = CLIP((Y2 + ushift));
      g2 = CLIP(( ((Y2<<8) - (77*r2) -(26*b2) ) * 427 ) >> 16);
#endif
#if 1
      /* IJG's  method */

      r1 = CLIP((Y1 + (((359*cr) - 45645) >> 8)));
      b1 = CLIP((Y1 + (((454*cb) - 57838) >> 8)));
      g1 = CLIP(( (Y1<<8) - (88 * cb) - (183 * cr) + 34542) >> 8);

      r2 = CLIP((Y2 + (((359*cr) - 45645) >> 8)));
      b2 = CLIP((Y2 + (((454*cb) - 57838) >> 8)));
      g2 = CLIP(( (Y2<<8) - (88 * cb) - (183 * cr) + 34542) >> 8);
#endif

      *destptr++ = (unsigned char) (255-r1);
      *destptr++ = (unsigned char) (255-g1);
      *destptr++ = (unsigned char) (255-b1);
      *destptr++ = (unsigned char) A1;
      *destptr++ = (unsigned char) (255-r2);
      *destptr++ = (unsigned char) (255-g2);
      *destptr++ = (unsigned char) (255-b2);
      *destptr++ = (unsigned char) A2;
    }

  memcpy(dataBuffer, tempBuffer,oneTile_inBytes);

  return TRUE;

}  /* dJPEG_Upsample4224AndConvert() */


static int dJPEG_YCbCrToRGB(unsigned char *dataBuffer,  size_t oneTile_inBytes,
                            void *decoder)
{
  /*
  ** converts YCbCr data to RGB data by following formulae:
  ** U = (Cb - 0.5) *2;
  ** V = (Cr - 0.5) *1.6;
  ** R = V + Y;
  ** G = (Y - 0.3R - 0.1B)/0.6;
  ** B = U + Y;
  */

  DECODER_STRUCT *decodePtr = (DECODER_STRUCT *)decoder;
  int num_channels = decodePtr->num_channels;
  int r,g,b;
  int y, cb,cr;
  unsigned char *tempPtr, *srcPtr;
  unsigned long i;

  srcPtr = dataBuffer;
  tempPtr = dataBuffer;

  /* if it is a 3-channel, rotate. if it is 4-channel (has an alpha channel),
     just ignore the 4th byte.
  */
  for (i=0; i<oneTile_inBytes; i+=num_channels)
    {
      y = *srcPtr++;
      cb = *srcPtr++;
      cr = *srcPtr++;
#if 0
      /* from pink book, section 2.4.42: */
      u = (cb - 128) << 9;
      v = (cr  - 128) * 410;
      r = CLIP((y + (v >> 8)));
      b = CLIP((y + (u >> 8)));
      g = CLIP(( ((y<<8) - (77*r) -(26*b) ) * 427 ) >> 16);
#endif
#if 1
      /* IJG's numbers : */
      r = CLIP((y + (((359*cr) - 45645) >> 8)));
      b = CLIP((y + (((454*cb) - 57838) >> 8)));
      g = CLIP(( (y<<8) - (88 * cb) - (183 * cr) + 34542) >> 8);
#endif    
      /* if a 4-channel image, invert the r,g,b channels...RFC 63 of
         Kodak .
      */
      if (num_channels == 4){
        *tempPtr++ = (unsigned char) (255 - r);
        *tempPtr++ = (unsigned char) (255 - g);
        *tempPtr++ = (unsigned char) (255 - b);
        *tempPtr++ = (unsigned char) (*srcPtr++); /* the alpha channel */
      }
      else {
        *tempPtr++ = (unsigned char) r;
        *tempPtr++ = (unsigned char) g;
        *tempPtr++ = (unsigned char) b;
      }
    }

  return TRUE;

} /* dJPEG_YCbCrToRGB() */
