/* -*-C-*-
******************************************************************************
*
* File:         public.c
* RCS:          $Header$
* Description:  A FlashPix-friendly interface for the HP JPEG Encoder.
* Author:       Kirt Winter
* Created:      Fri Mar  1 11:04:22 1995
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
#define IMP_ENCODER
#include <stdlib.h>
#include <string.h>
#include "jpegconf.h"

#include "enctile.h"
#include "encoder.h"
#include "public.h"
#include "process.h"
#include "fpxmem.h"

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

/*
  The simplest JPEG scenario involves RGB data (no alpha) passed to the
  encoder.  With calls to eJPEG_Init, then eJPEG_InitCompression, then
  eJPEG_EncodeTile, you get no subsampling, internal RGB->YCbCr, Q-Factor
  of 50, internal Huffman and Q-tables.  Other routines allow you to
  disable some or all of the internal subsampling and YCbCr conversions.

  A single call to eJPEG_DisableInternalYCbCr allows opponent color spaces
  (like FlashPix-YCC) to be used.  This new sequence would be eJPEG_Init,
  eJPEG_DisableInternalYCbCr, eJPEG_InitCompression, then eJPEG_EncodeTile.
  
  All modifications to the encoder's behavior come between the calls
  eJPEG_Init and eJPEG_.
*/

/*
** No matter what else you do, you have to call this. 
*/
int  eJPEG_Init(void **encoder)
{

  ENCODER_STRUCT  *jpg ;
  int x;

  if(!( *encoder = (ENCODER_STRUCT *)FPX_malloc( sizeof(ENCODER_STRUCT))))
    return eJPEG_MEMORY_ERROR;
  else
  {
    jpg = (ENCODER_STRUCT *)*encoder;
    {
      jpg->header = NULL;
      jpg->headerBytes = 0;
      jpg->ssDisabled = FALSE;    /* internal subsampling is enabled */
      jpg->YCrCbDisabled = FALSE; /* internal YCrCb is enabled */
      jpg->xPixels = 64;      /* defaulted to FlashPix baseline */
      jpg->yPixels = 64;      /* ditto */
      jpg->bytes = 3;         /* RGB is assumed */
      jpg->scratch = (unsigned char *)
          FPX_malloc(jpg->xPixels * jpg->yPixels * jpg->bytes);
      
      if(jpg->scratch == NULL)
        return eJPEG_MEMORY_ERROR;  /* CHG_JPEG_MEM_FIX - added test */

      jpg->subsampling = 0x11;    /* no subsampling */

      /* init iHsamp & iVsamp to reflect above defaults */
      for(x = 0; x < 3; x++)
        jpg->iHsamp[x] = jpg->iVsamp[x] = 1;

      jpg->iHsamp[3] = jpg->iVsamp[3] = 0;
      jpg->qFactor = 50;     /* defaulted to 50 */
      jpg->interleaveType = 0;   /* interleaved */

      SetDefaultTables((void *) jpg);

      return eJPEG_NO_ERROR;
    }
  }
  return eJPEG_MEMORY_ERROR;
}

/*
 If you want to do any required subsampling, call this next.  NOTE:  Internal
 subsampling will only be performed if you later call eJPEG_SetSubsampling with
 arguments that will call for it.
 Once more, with feeling...
 By disabling internal subsampling, you're telling the eJPEG_EncodeTile
 function that tile data passed to it has already been subsampled correctly,
 and it is a very trusting function, so don't lie.
*/
int eJPEG_DisableInternalSubsampling(void  *encoder)
{
  ENCODER_STRUCT *jpg = (ENCODER_STRUCT *)encoder;

  if (jpg != NULL){
    jpg->ssDisabled = TRUE;
    return(eJPEG_NO_ERROR);
  }
  else
    return(eJPEG_INVALID_ENCODER);
}

/*
 If you already have opponent color data (like FlashPix-YCC), call this.  You might
 also want to pass RGB data to the encoder (in which case, you probably don't want
 it subsampled either, see above), or for some strange reason you might want to
 do the YCbCr conversion yourself.  In any case, be sure to set the "Internal
 color conversion" field of the compression subtype correctly.
*/
int eJPEG_DisableInternalYCbCr(void  *encoder)
{
  ENCODER_STRUCT *jpg = (ENCODER_STRUCT *)encoder;

  if(jpg != NULL){
    jpg->YCrCbDisabled = TRUE;
    return(eJPEG_NO_ERROR);
  }
  else
    return(eJPEG_INVALID_ENCODER);
}

/*
 Pass the same value to this function as you would put in the compression subtype's
 "Chroma subsampling."  Note, however, that this particular encoder will not support
*/
int eJPEG_SetSubsampling(
void *encoder,    /* same value returned by eJPEG_Init */
unsigned char subSampling  /* pass the same value you'd put in the FlashPix 
                                  compression subtype field for JPEG data */
)
{
  ENCODER_STRUCT *jpg = (ENCODER_STRUCT *)encoder;
  int ret, x;

  if(jpg != NULL){
    if((subSampling == 0x11) || (subSampling == 0x22) || (subSampling == 0x21) )
    {
      for(x = 0; x < jpg->bytes; x++)
        jpg->iHsamp[x] = jpg->iVsamp[x] = 1; /* default */
      if((jpg->bytes == 4) && (subSampling == 0x22))
      {
        jpg->subsampling = subSampling;
        jpg->iHsamp[0] = jpg->iVsamp[0] = 2;
        jpg->iHsamp[3] = jpg->iVsamp[3] = 2; /* alpha channel */
        ret = eJPEG_NO_ERROR;
      }
      else
      {
        if(jpg->bytes == 3)
          jpg->iHsamp[3] = jpg->iVsamp[3] = 0;
        if(subSampling == 0x22)
          jpg->iHsamp[0] = jpg->iVsamp[0] = 2;
        else if (subSampling == 0x21){
          jpg->iHsamp[0] = 2;
          jpg->iVsamp[0] = 1;
        }
        jpg->subsampling = subSampling;
        ret = eJPEG_NO_ERROR;
      }
    }
    else
      ret = eJPEG_UNSUPPORTED_SUBSAMPLING;

    return(ret);
  }
  else
    return(eJPEG_INVALID_ENCODER);
}

int eJPEG_EnableInternalYCbCr(void  *encoder)
{
  ENCODER_STRUCT *jpg = (ENCODER_STRUCT *)encoder;

  if(jpg != NULL ){
    jpg->YCrCbDisabled = FALSE; /* not sure if this will work */
    return(eJPEG_NO_ERROR);
  }
  else
    return(eJPEG_INVALID_ENCODER);
}


/* default is for channel interleave to be on, hence there is
   no need to call this routine if the interleave has never
   been disabled.
*/
int eJPEG_EnableChannelInterleave(void  *encoder)
{
  ENCODER_STRUCT *jpg = (ENCODER_STRUCT *)encoder;

  if(jpg != NULL ){
    jpg->interleaveType = 0; /* channels are interleaved */
    return(eJPEG_NO_ERROR);
  }
  else
    return(eJPEG_INVALID_ENCODER);
}

int eJPEG_DisableChannelInterleave(void  *encoder)
{
  ENCODER_STRUCT *jpg = (ENCODER_STRUCT *)encoder;

  if(jpg != NULL ){
    jpg->interleaveType = 1;
    return(eJPEG_NO_ERROR);
  }
  else
    return(eJPEG_INVALID_ENCODER);
}

/* Call this routine to change from the default Q factor. */
int eJPEG_SetQFactor(
void *encoder,   /* same value returned by eJPEG_Init */
int quality  /* default is 50 */
)
{
  ENCODER_STRUCT *jpg = (ENCODER_STRUCT *)encoder;

  if(jpg != NULL){
    if (quality > 255)
      quality = 255;
    else if (quality < 0)
      quality = 0;
    jpg->qFactor = quality;
    return(eJPEG_NO_ERROR);
  }
  else
    return(eJPEG_INVALID_ENCODER);
}

/* Call this routine if you want to use custom Huffman Tables.
   The custom tables are specified in the HuffTables array of
   huffman tables. The tables in that array should alternate
   DC-AC-DC-AC ... (a maximum of eight tables).Be careful with
   the 'ident' values set.  JPEG requires that these values be 
   0,1,2,or 3 for extended JPEG systems, but only 0 or 1 for 
   baseline JPEG systems.
*/
int eJPEG_SetHuffTables(void *encoder, 
int NumbHuffTables, 
JPEGHuffTable **HuffTables, 
unsigned char * CompDCHuffIdent,
unsigned char * CompACHuffIdent)
{
  ENCODER_STRUCT* jpg = (ENCODER_STRUCT *)encoder;
  JPEGHuffTable *hufftable;
  int i;

  if (((NumbHuffTables%2)!=0) || (NumbHuffTables> 8) || (NumbHuffTables < 2))
    return(eJPEG_BAD_HUFFMAN_TABLE);

  jpg->nu_huff = NumbHuffTables;
  hufftable = (JPEGHuffTable *) HuffTables;
  for (i=0; i < NumbHuffTables; i++, hufftable++) {
    jpg->HuffTables[i].bits = hufftable->bits;
    jpg->HuffTables[i].vals = hufftable->vals;
    jpg->HuffTables[i].hclass = hufftable->hclass;
    jpg->HuffTables[i].ident = hufftable->ident;
  }
  for (i=NumbHuffTables; i < 8 ;i++) {
    jpg->HuffTables[i].bits = NULL;
    jpg->HuffTables[i].vals = NULL;
    jpg->HuffTables[i].hclass = 0;
    jpg->HuffTables[i].ident = 0;
  }
  for (i=0; i < 4; i++) {
    jpg->CompDCHuffIdent[i] = CompDCHuffIdent[i];
    jpg->CompACHuffIdent[i] = CompACHuffIdent[i];
  }

  return(eJPEG_NO_ERROR);
}


/* Call this routine if you want to use custom Quantization Tables.
   The custom tables are specified in the QuantTables array of
   QuantTables tables. The ident field of these tables 
   should have the values 0,1,2 or 3.
*/
int eJPEG_SetQuantTables(void* encoder, 
int NumbQuantTables, JPEGQuantTable **QuantTables, 
unsigned char* CompQuantIdent)
{
  ENCODER_STRUCT* jpg = (ENCODER_STRUCT *)encoder;
  JPEGQuantTable *quant;
  int i;

  if (NumbQuantTables > 4 || NumbQuantTables < 1)
    return(eJPEG_BAD_QUANT_TABLE);

  jpg->nu_qtables = NumbQuantTables;

  for (i=0, quant=(JPEGQuantTable *) QuantTables; i < NumbQuantTables; i++, quant++) {
    jpg->QuantTables[i].quantizer = quant->quantizer;
    jpg->QuantTables[i].ident = quant->ident;

  }
  for (i=NumbQuantTables; i < 4 ;i++) {
    jpg->QuantTables[i].quantizer = NULL;
    jpg->QuantTables[i].ident = 0;
  }
  for (i=0; i < 4; i++) {
    jpg->CompQuantIdent[i] = CompQuantIdent[i];
  }

  return(eJPEG_NO_ERROR);
}

int eJPEG_SetTileSize(
void *encoder,
int hSize,     /* in pixels, the width of a tile */
int vSize,     /* in pixels, the height of a tile */
int bytesPerPixel  /* how many bytes per pixel */
)
{
  ENCODER_STRUCT *jpg = (ENCODER_STRUCT *)encoder;
  int ret, x;

  if(jpg != NULL ){
    if((bytesPerPixel > 4) || (bytesPerPixel < 1))
      ret = eJPEG_UNSUPPORTED_BYTES_PER_PIXEL;
    else{
      jpg->xPixels = hSize;
      jpg->yPixels = vSize;
      jpg->bytes = bytesPerPixel;

      if (jpg->scratch != NULL) FPX_free(jpg->scratch);
      jpg->scratch = 
          (unsigned char *) FPX_malloc(hSize * vSize * bytesPerPixel);
      
      if (jpg->scratch == NULL)     /* CHG_JPEG_MEM_FIX - added check */
        return (eJPEG_MEMORY_ERROR);

      for(x = 0; x < jpg->bytes; x++)
        jpg->iHsamp[x] = jpg->iVsamp[x] = 1;

      /* Subsampling of 1 and 2-channel images is not supported, so   */
      /* disable internal subdsampling. Note that the FlashPix spec */
      /* does not specify what subsampling values are valid for these */
      /* cases, so we simply ignore them via the following override.  */
      if((jpg->bytes < 3) && (jpg->subsampling > 0x11)){
        jpg->subsampling = 0x11;
        ret = eJPEG_NO_ERROR;
      }
      else if (jpg->subsampling == 0x22)
      {
        if (jpg->bytes == 4)  {
          jpg->iHsamp[0] = jpg->iVsamp[0] = 2;
          jpg->iHsamp[3] = jpg->iVsamp[3] = 2;
        }
        else {
          jpg->iHsamp[0] = jpg->iVsamp[0] = 2;
        }
        ret = eJPEG_NO_ERROR;
      }
      else if (jpg->subsampling == 0x21)
      {
        if (jpg->bytes == 4)  {
          jpg->iHsamp[0] = 2;
          jpg->iHsamp[3] = 2;
        }
        else {
          jpg->iHsamp[0] = 2;
        }
        jpg->iHsamp[0] = 2;
        ret = eJPEG_NO_ERROR;
      }
      else { /* must be non-subsampled (0x11) */

        ret = eJPEG_NO_ERROR;
      }
    }

    return(ret);
  }
  else
    return(eJPEG_INVALID_ENCODER);

}

/*
 The next two functions are an either/or situation.  If you want the
 data in each tile to be a fully compliant JFIF stream, you call
 eJPEG_ConcatenateHeader.  If you want to store the header separately
 from all the tiles, you call eJPEG_CreateHeader, which will return
 you a header that you can store in the JPEG Tables section of one
 of the far too numerous FlashPix property sets (compression information
 property group).  Note that it is your responsibility to make certain 
 you put the proper ID in the JPEG tables selector of the tile's
 compression subtype field.
*/
int eJPEG_CreateHeader(
void *encoder,       /* same value returned by eJPEG_Init */
long hdrBufferSize,      /* the size of the <hdrBuffer> in bytes */
unsigned char *hdrBuffer, /* the buffer itself */
long *hdrBufferUsed      /* upon return shows the amount of
          <hdrBuffer> that was used */
)
{
  ENCODER_STRUCT *jpg = (ENCODER_STRUCT *)encoder;

  if(jpg != NULL ){

    JPEGEncodeTileInit(
        NULL,
        jpg->xPixels,
        jpg->yPixels,
        jpg->bytes,    /* number of bytes per pixel */
    jpg->qFactor,
        jpg->iHsamp,
        jpg->iVsamp, /* Hor. & Vert. subsampling factors */
    jpg->interleaveType,   
        &jpg->tile_data,
        jpg->nu_huff, /* # of Huffman Tables (a max of 8 tables, ie four DC-AC sets) */
    jpg->HuffTables,
        jpg->CompDCHuffIdent,
        jpg->CompACHuffIdent,
        jpg->nu_qtables, /* # of Q-tables (a max of 4 tables) */
    jpg->QuantTables,
        jpg->CompQuantIdent,
        &jpg->jpegStruct,
        hdrBuffer,
        hdrBufferSize,
        hdrBufferUsed);

    return(eJPEG_NO_ERROR);
  }
  else
    return(eJPEG_INVALID_ENCODER);
}

/* see explanation above  */
#define HDR_BUFF_SIZE 1440
int eJPEG_ConcatenateHeader(
void *encoder,   /* same value returned by eJPEG_Init */
unsigned char *hdrBuffer, /* the buffer itself */
long *hdrBufferUsed      /* upon return shows the amount of
          <hdrBuffer> that was used */
)
{
  int ret;
  long size,size_minus_eoi;
  unsigned char tempBuf[HDR_BUFF_SIZE];
  ENCODER_STRUCT *jpg = (ENCODER_STRUCT *)encoder;

  ret = eJPEG_CreateHeader(encoder, HDR_BUFF_SIZE, tempBuf, &size);
  size_minus_eoi = size-2;
  *hdrBufferUsed = size;

  if(ret)
    return(ret);

  if(jpg != NULL ){
    jpg->headerBytes = size_minus_eoi;
    jpg->header = (unsigned char *) FPX_malloc(size);
    if (memcpy(jpg->header, tempBuf, (size_t) size_minus_eoi))
      ret = eJPEG_NO_ERROR;
    else
      ret = eJPEG_MEMORY_ERROR;

  }

  hdrBuffer = jpg->header;

  return(ret);
}

long eJPEG_EncodeTile(
void *encoder,    /* same value returned by eJPEG_Init */
unsigned char *inbuf, /* assumed to be the size of a tile or sub-
           sampled tile! */
unsigned char *outbuf,  /* the buffer to put the compressed tile into */
size_t outbuf_size  /* size of the output buffer */
)
{
  int check;
  long finalSize;
  size_t inBufSize;
  long offset;
  ENCODER_STRUCT *jpg = (ENCODER_STRUCT *)encoder;

  if(jpg != NULL ){
    inBufSize = jpg->xPixels * jpg->yPixels * jpg->bytes;

    /* initialize output buffer to input buffer (needed for
     no color rotation, no subsample case)
  */
    if (inBufSize < outbuf_size) 
      return (-1);
    memcpy(jpg->scratch, inbuf, inBufSize);

    /* decide whether or not we need to subsample and/or YCrCb */
    if(!jpg->YCrCbDisabled)
    {
      if(!jpg->ssDisabled && (jpg->subsampling == 0x22)){
        /* subsample and YCrCb  */
        RGBtoYCrCb_SubSample411(
            (unsigned char *) inbuf, 
            (unsigned char *) jpg->scratch,
            jpg->xPixels, 
            jpg->bytes);
        if (jpg->bytes == 3)
          inBufSize = inBufSize >> 1;
        else if (jpg->bytes == 4)
          inBufSize = 10 * inBufSize / 16;
      }
      else if (!jpg->ssDisabled && (jpg->subsampling == 0x21)) {
        RGBtoYCrCb_SubSample422(
            (unsigned char *) inbuf, 
            (unsigned char *) jpg->scratch,
            jpg->xPixels, 
            jpg->bytes);
        if (jpg->bytes == 3)
          inBufSize = 4 * inBufSize / 6;
        else if (jpg->bytes == 4)
          inBufSize = 6 * inBufSize / 8;
      }
      else {
        /* just YCrCb it */
        RGBtoYCrCb(
            (unsigned char *) inbuf, 
            (unsigned char *) jpg->scratch,
            jpg->xPixels, 
            jpg->bytes);
      }
    }
    else { /* don't YCrCb rotate */

      if(!jpg->ssDisabled && (jpg->subsampling == 0x22)){
        /* just subsample it */
        SubSample411(
            (unsigned char *) inbuf, 
            (unsigned char *) jpg->scratch,
            jpg->xPixels, 
            jpg->bytes);
        if (jpg->bytes == 3)
          inBufSize = inBufSize >> 1;
        else if (jpg->bytes == 4)
          inBufSize = 10 * inBufSize / 16;
      }
      else if (!jpg->ssDisabled && (jpg->subsampling == 0x21)) {
        SubSample422(
            (unsigned char *)inbuf, 
            (unsigned char *) jpg->scratch,
            jpg->xPixels, 
            jpg->bytes);
        if (jpg->bytes == 3)
          inBufSize = 4 * inBufSize / 6;
        else if (jpg->bytes == 4)
          inBufSize = 6 * inBufSize / 8;
      }
    }
    jpg->tile_data.data = jpg->scratch;
    offset = (jpg->headerBytes != 0) ? -2 : 0;

    /* jpg->headerBytes != 0 means that header & entropy data are 
     in the same jfif bitstream, so the EOI-SOI markers at the 
     end of the header and the beginning of the entropy bitstreams
     need to be striped off.
  */
    check = JPEGEncodeTile(
        &jpg->tile_data,
        &jpg->jpegStruct,
        jpg->CompDCHuffIdent,
        jpg->CompACHuffIdent,
        jpg->CompQuantIdent,
        outbuf + jpg->headerBytes + offset,
        jpg->xPixels * jpg->yPixels * jpg->bytes,
        &finalSize);

    /* Are supposed to include the header in the tile */
    if(jpg->headerBytes){
      memcpy(outbuf, jpg->header, (size_t)jpg->headerBytes);
      finalSize += (jpg->headerBytes - 2);

      /* adjusted by the size of the EOI-SOI markers which
         have been striped off */
    }
    /* notice that the SOI in the entropy bitstream will be overwritten by 
     the last two bytes of the header bitstream */

  }
  return(finalSize);
}

/* Once you are finished with the encoder, you have to call this. */
int eJPEG_Shutdown(void* encoder)
{
  ENCODER_STRUCT *jpg = (ENCODER_STRUCT *)encoder;

  if( jpg )
  {
    if(jpg->header) {
      FPX_free(jpg->header);
      jpg->header = NULL;           /* CHG_JPEG_MEM_FIX - clear ptr */
    }
    if(jpg->scratch) {
      FPX_free(jpg->scratch);
      jpg->scratch = NULL;          /* CHG_JPEG_MEM_FIX - clear ptr */
    }
    FPX_free(jpg);
    jpg = 0;                  /* CHG_JPEG_MEM_FIX - clear ptr */

    return(TRUE);
  }

  return(FALSE);

} /* eJPEG_Shutdown() */
