/* -*-C-*-
******************************************************************************
*
* File:         encoder.h
* RCS:          $Header$
* Description:  A Flashpix-friendly interface for the HP JPEG Encoder.
* Author:       Kirt Winter
* Created:      Fri Mar  1 11:09:55 1995
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

#ifdef __cplusplus
extern "C" {
#endif

/*************************************************************************
The simplest JPEG scenario involves RGB data (no alpha) passed to the
encoder.  With calls to eJPEG_Init, then eJPEG_CreateHeader (or
eJPEG_ConcatenateHeader if you want to embed the JPEG header in the tile
data), then eJPEG_EncodeTile, you get no subsampling, internal RGB->YCbCr,
Q-Factor of 50, internal Huffman tables.  Other routines allow you to
disable some or all of the internal subsampling and YCbCr conversions.

A single call to eJPEG_DisableInternalYCbCr allows opponent color spaces
(like Photo-YCC) to be used.  This new sequence would be eJPEG_Init,
eJPEG_DisableInternalYCbCr, eJPEG_CreateHeader, then eJPEG_EncodeTile.

All modifications to the encoder's behavior come between the calls
eJPEG_Init and eJPEG_.
**************************************************************************/

/* No matter what else you do, you have to call this. */
JPEGEXPORT
int eJPEG_Init(void **encoder); /* save the return value for later use */

/*
If you want to do any required subsampling, call this next.  NOTE:  Internal
subsampling will only be performed if you later call eJPEG_SetSubsampling with
arguments that will call for it.
Once more, with feeling...
By disabling internal subsampling, you're telling the eJPEG_EncodeTile
function that tile data passed to it has already been subsampled correctly.
*/
JPEGEXPORT
 int eJPEG_DisableInternalSubsampling(void *encoder);


/* If you already have opponent color data (like FPX-YCC), call this.  You might
 also want to pass RGB data to the encoder (in which case, you probably don't want
 it subsampled either, see above), or for some strange reason you might want to
 do the YCbCr conversion yourself.  In any case, be sure to set the "Internal
 color conversion" field of the compression subtype correctly.
*/
JPEGEXPORT
 int eJPEG_DisableInternalYCbCr(void  *encoder);


/* Pass the same value to this function as you would put in the compression subtype's
 "Chroma subsampling."  Note, however, that this particular encoder will not support
*/ 
JPEGEXPORT
 int eJPEG_SetSubsampling(
void *encoder,    /* same value returned by eJPEG_Init */
unsigned char subSampling  /* pass the same value you'd put in the FlashPix
          compression subtype field for JPEG data */
);


/* Call this routine to enable internal color conversion (the default).
 Reason to use this function is if different tiles have different color space
 properties, and we need to reenable the color rotation after disabling it
 for prior tiles.
*/
JPEGEXPORT
 int eJPEG_EnableInternalYCbCr( void *encoder );


/* default is to have channel interleave enabled. Call this
   routine only if channel interleave has been disabled
   for a previous tile.
*/
JPEGEXPORT
 int eJPEG_EnableChannelInterleave(void  *encoder); 


JPEGEXPORT
 int eJPEG_DisableChannelInterleave(void  *encoder);


/* Call this routine to change from the default Q factor. */
JPEGEXPORT
 int eJPEG_SetQFactor(
void *encoder, /* same value returned by eJPEG_Init */
int quality    /* default is 50 */
);

/* Call this routine if you want to use custom Huffman Tables.
   The custom tables are specified in the HuffTables array of
   huffman tables. The tables in that array should alternate
   DC-AC-DC-AC ... (a maximum of eight tables).Be careful with
   the 'ident' values set.  JPEG requires that these values be
   0,1,2,or 3 for extended JPEG systems, but only 0 or 1 for
   baseline JPEG systems.
*/
JPEGEXPORT
 int eJPEG_SetHuffTables(
void *encoder, 
int NumbHuffTables, 
JPEGHuffTable **HuffTables, 
unsigned char * CompDCHuffIdent,
unsigned char * CompACHuffIdent);


/* Call this routine if you want to use custom Quantization Tables.
   The custom tables are specified in the QuantTables array of
   QuantTables tables. The ident field of these tables
   should have the values 0,1,2 or 3.
*/
JPEGEXPORT
 int eJPEG_SetQuantTables(
void* encoder, 
int NumbQuantTables, 
JPEGQuantTable **QuantTables, 
unsigned char *CompQuantIdent);

/* Note that currently, if you set 4-bytes per pixel, you don't get subsampling */
JPEGEXPORT
 int eJPEG_SetTileSize(
void *encoder,  /* same value returned by eJPEG_Init */
int hSize,    /* in pixels, the width of a tile */
int vSize,    /* in pixels, the height of a tile */
int bytesPerPixel /* how many bytes per pixel */
);


/* The next two functions are an either/or situation.  If you want the
   data in each tile to be a fully compliant JFIF stream, you call
   eJPEG_ConcatenateHeader.  If you want to store the header separately
   from all the tiles, you call eJPEG_CreateHeader, which will return
   you a header that you can store in the JPEG Tables section of one
   of the far too numerous FlashPix property sets (compression information
   property group).  Note that putting the proper ID in the JPEG tables
   selector of the tile's compression subtype field is your job.
*/ 
JPEGEXPORT
 int eJPEG_CreateHeader(
void *encoder,       /* same value returned by eJPEG_Init */
long hdrBufferSize,      /* the size of the <hdrBuffer> in bytes */
unsigned char *hdrBuffer,/* the buffer itself */
long *hdrBufferUsed      /* upon return shows the amount of 
              hdrbuffer that was used */
);


/* see above */
JPEGEXPORT
 int eJPEG_ConcatenateHeader( 
void *encoder,
unsigned char *hdrBuffer,/* the buffer itself */
long *hdrBufferUsed      /* upon return shows the amount of 
        hdrbuffer that was used. */
);


JPEGEXPORT
 long eJPEG_EncodeTile(
void *encoder,    /* same value returned by eJPEG_Init */
unsigned char *inbuf, /* assumed to be the size of a tile or sub-
                             sampled tile! */
unsigned char *outbuf,/* the buffer to put the compressed tile into */
size_t outbuf_size    /* size of the output buffer */
);

/* called at the end of using the encoder project */
JPEGEXPORT
 int eJPEG_Shutdown(void* encoder);

#ifdef __cplusplus
}
#endif

/* An internal routine called form eJPEG_Init() */
int SetDefaultTables(void* encoder);

/* CHG_FILE_ERR - unified JPEG error values by offsetting the values by 0x0200 */
#define eJPEG_NO_ERROR            0
#define eJPEG_SUBSAMPLING_NOT_SUPPORTED   0x0201
#define eJPEG_INVALID_ENCODER       0x0202
#define eJPEG_UNSUPPORTED_SUBSAMPLING   0x0203
#define eJPEG_UNSUPPORTED_BYTES_PER_PIXEL 0x0204
#define eJPEG_MEMORY_ERROR          0x0205
#define eJPEG_BAD_HUFFMAN_TABLE             0x0206
#define eJPEG_BAD_QUANT_TABLE               0x0207

