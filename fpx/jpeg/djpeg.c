/* -*-C-*-
******************************************************************************
*
* File:         djpeg.c
* RCS:          $Header$
* Description:  main module of JPEG decoder library.
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

#include <stdio.h>
#include <stdlib.h>
#include "jpegconf.h"
#include "decoder.h"
#include "dbuffer.h"
#include "dectile.h"
#include "djpeg.h"
#include "fpxmem.h"

void  DB_Initdb_state(DB_STATE *db_state);

/********************************** Local  **********************************/

int Read_Bytes_From_Memory(DB_STATE *db_state,
unsigned char *buf,
int nbytes)
{
  int i;

  /* Check if there is enough data in the compressed-data buffer (inbuf) */
  if (db_state->nbytes_left < nbytes) {
    nbytes = (db_state->nbytes_left <= 0) ? 0 : (int) db_state->nbytes_left;
  }
  for (i = nbytes; i > 0; i--) *buf++ = *db_state->db_inptr++;
  db_state->nbytes_left -= nbytes;
  return(nbytes);
}

void Write_Rows_To_Memory(unsigned char *membuf,
unsigned char *data,
int nrows,
int nbytes_per_row)
{
  register int i,j;
  unsigned char *in_ptr,*out_ptr;

  in_ptr=data; 
  out_ptr=membuf;
  for (i=nrows; i > 0 ;i--)
    for (j=nbytes_per_row; j > 0 ;j--)
      *out_ptr++ = *in_ptr++;
}


void  DB_Initdb_state(DB_STATE *db_state)
{
  /* just null out all pointers in db_state */
  db_state->db_buf = NULL;
  db_state->db_ptr = NULL;
  db_state->db_supplied_bufs = NULL;
  db_state->db_supplied_ptr = NULL;
  db_state->db_outbuf = NULL;
  db_state->db_outptr = NULL;
  db_state->db_inptr = NULL;
  db_state->db_inbuf = NULL;
  db_state->db_MCUbuf = NULL;
  db_state->db_MCUptr = NULL;
  db_state->db_current_buf = NULL;
  db_state->db_current_buf2 = NULL;
  db_state->db_xtra_buf = NULL;
  db_state->db_row_bufs = NULL;

} /* DB_Initdb_state() */


/********************************** EXPORT **********************************/

int JPEGDecodeTileInit(
unsigned char *inbuf,
size_t inbuf_size,
TILE_DATA *tile_data,
size_t *header_size,
void *decoder
)
{
  int method;
  int error_type;
  DB_STATE *db_state;
  DECODER_STRUCT *decodePtr = (DECODER_STRUCT *)decoder;
  int grayscale;

  method=0;
  grayscale = 0;

  if ((db_state= (DB_STATE *) FPX_malloc(sizeof(DB_STATE)))== NULL) {
    return(ERROR_MEM);
  }

  DB_Init(db_state, (int (*)(void *,void *,int))Read_Bytes_From_Memory, 
      NULL, inbuf, inbuf_size, 
      NULL, tile_data->iflag, 1);
  error_type = DE_Decode(db_state, decodePtr, method, grayscale, 1, &tile_data->width,
      &tile_data->height, &tile_data->components,
      tile_data->Hsamp, tile_data->Vsamp, tile_data->iflag );
  *header_size = (size_t) DB_Processed_Bytes(db_state);
  DB_End(db_state);

  FPX_free(db_state); 
  db_state = NULL;
  return(error_type);
}


int dJPEG_DecodeTileHeader(unsigned char *inbuf,
size_t inbuf_size,
void *decoder,
int dct_method)
{
  int method;
  int grayscale;
  int error_type;
  size_t byte_count;
  DB_STATE *db_state;
  DECODER_STRUCT *decodePtr = (DECODER_STRUCT *)decoder;
  int pass_num;
  size_t outbuf_size;
  int width,height,components;
  int iflag = 0;
  int Hsamp[4],Vsamp[4];

  method=dct_method;
  if ((method != 0) && (method != 1)) method = 0;
  grayscale = 0;

  if ((db_state= (DB_STATE *) FPX_malloc(sizeof(DB_STATE)))== NULL) {
    return(ERROR_MEM);
  }
  db_state->db_buf = NULL;

  outbuf_size = inbuf_size;
  if ((db_state->db_MCUbuf = (unsigned int *)FPX_calloc(1, outbuf_size*sizeof(int)))==NULL) {
    return(ERROR_MEM);
  }
  db_state->db_MCUptr = db_state->db_MCUbuf;

  /* Interpret part of JPEG-Header */
  pass_num = 1;
  DB_Init(db_state, (int(*)(void *, void *, int))Read_Bytes_From_Memory, 
      NULL, inbuf, inbuf_size, 
      NULL, iflag, pass_num);
  error_type = DE_Decode(db_state, decodePtr, method, grayscale, pass_num, 
      &width, &height, &components,
      Hsamp,Vsamp, iflag );
  if (error_type) {
    return(error_type);
  }
  byte_count = (size_t)DB_Processed_Bytes(db_state);

  if (db_state->db_MCUbuf != NULL) {
    FPX_free(db_state->db_MCUbuf); 
    db_state->db_MCUbuf=NULL;
  }
  DB_End(db_state);
  FPX_free(db_state); 
  db_state = NULL;

  return(error_type);
}


int dJPEG_DecodeTile(unsigned char *outbuf,
size_t outbuf_size,
unsigned char *inbuf,
size_t inbuf_size,
void *decoder,
int parse_option, 
int dct_method)
{
  int method;
  int grayscale;
  int error_type;
  size_t byte_count;
  DB_STATE *db_state;
  DECODER_STRUCT *decodePtr = (DECODER_STRUCT *)decoder;
  int pass_numb;
  TILE_DATA tile_data;
  int hdim[4], vdim[4];

  /* jpeg interleave flag of 1 means it is interleaved.
     FlashPix interleave  flag of 0 means it is interleaved! */
  tile_data.iflag =  (decodePtr->interleave_type == 0)? 1 : 0;
  tile_data.Hsamp = hdim;
  tile_data.Vsamp = vdim;

  if (parse_option == ENTROPY_AND_HEADER) {
    method=dct_method;
    if ((method != 0) && (method != 1) && (method !=2)) method = 0;
    grayscale = 0;

    if ((db_state= (DB_STATE *) FPX_malloc(sizeof(DB_STATE)))== NULL) {
      return(ERROR_MEM);
    }

    DB_Initdb_state(db_state);

    if ((db_state->db_MCUbuf = 
        (unsigned int *)FPX_calloc(1, outbuf_size*sizeof(int)))==NULL) {
      return(ERROR_MEM);
    }
    db_state->db_MCUptr = db_state->db_MCUbuf;

    pass_numb = 2;
    DB_Init(db_state, (int(*)(void *, void *, int))Read_Bytes_From_Memory, 
        (void(*)(void *, void *, int, int))Write_Rows_To_Memory, 
        inbuf, inbuf_size, outbuf, tile_data.iflag, pass_numb);
    error_type = DE_Decode(db_state, decodePtr, method, 
        grayscale, pass_numb, &tile_data.width,
        &tile_data.height, &tile_data.components,
        tile_data.Hsamp, tile_data.Vsamp, 
        tile_data.iflag);
    if (error_type) {
      return(error_type);
    }
    byte_count = (size_t)DB_Processed_Bytes(db_state);

    if (db_state->db_MCUbuf != NULL) {
      FPX_free(db_state->db_MCUbuf);
      db_state->db_MCUbuf=NULL;
    }
    DB_End(db_state);
    FPX_free(db_state);
    db_state = NULL;
  }
  else {  /* ENTROPY Data Only */
    method=dct_method;
    if ((method != 0) && (method != 1) && (method !=2)) method = 0;
    grayscale = 0;

    if ((db_state= (DB_STATE *) FPX_malloc(sizeof(DB_STATE)))== NULL) {
      return(ERROR_MEM);
    }

    DB_Initdb_state(db_state);

    if ((db_state->db_MCUbuf = 
        (unsigned int *)FPX_calloc(1, outbuf_size*sizeof(int)))==NULL) {
      return(ERROR_MEM);
    }
    db_state->db_MCUptr = db_state->db_MCUbuf;

    pass_numb = 0;
    DB_Init(db_state, (int(*)(void *, void *, int))Read_Bytes_From_Memory, 
        (void(*)(void *, void *, int, int))Write_Rows_To_Memory, 
        inbuf, inbuf_size, outbuf, tile_data.iflag, pass_numb);
    error_type = DE_Decode(db_state,decodePtr,method,grayscale, 
        pass_numb, 
        &tile_data.width,&tile_data.height, 
        &tile_data.components,
        tile_data.Hsamp, tile_data.Vsamp, 
        tile_data.iflag);
    if (error_type) {
      return(error_type);
    }

    byte_count = (size_t) DB_Processed_Bytes(db_state);

    if (db_state->db_MCUbuf != NULL) {
      FPX_free(db_state->db_MCUbuf);
      db_state->db_MCUbuf=NULL;
    }
    DB_End(db_state);
    FPX_free(db_state);
    db_state = NULL;
  } /* if ENTROPY_ONLY */

  /* decide whether upsampling/color rotation are needed and
      apply them if needed. Grayscale and 2-channel images can 
    never be downsampled or color-rotated.
   */
  if ((decodePtr->num_channels != 1) && (decodePtr->num_channels != 2)) {
    if ( (decodePtr != NULL)  && ((decodePtr->InternalUpsample) || 
        (decodePtr->InternalColor)))
      error_type = dJPEG_UpsampleAndConvert(decodePtr, outbuf, outbuf_size);
  }

  return(error_type);
}
