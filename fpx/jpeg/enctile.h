/* -*-C-*-
******************************************************************************
*
* File:         enctile.h
* RCS:          $Header$
* Description:  header file for JPEG encoder library.
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

typedef struct {
  unsigned char *bits;
  unsigned char *vals;
  unsigned char hclass; /* 0: DC, 1: AC */
  unsigned char ident;  /* 0,1,2,or 3 for extended JPEG systems */
                              /* 0, or 1 for baseline JPEG */
} JPEGHuffTable;

typedef struct {
  unsigned char *quantizer; /* Table elements specified in zigzag order */
  unsigned char ident;  /* 0,1,2,3 */
} JPEGQuantTable;

typedef struct {
    unsigned short ehufcode[256]; /* ehufcode[i]=Huffman code for value i */
    int ehufsize[256]; /* number of bits in ehufcode[i] */
} HUFFMAN_TABLE;       /* Internal Huffman-Table representation */


#ifndef TILE_DATA_DEFINED
typedef struct {
  int width;     /* tile width */
  int height;/* tile height */
  int components; /* # of components: 1-4 */
  int *Hsamp,*Vsamp; /* a pair of <Hsamp,Vsamp> subsampling factors per component */
  int iflag;     /* Interleave-flag: 1/0= Interleaved/Non-interleaved */
  unsigned char *data; /* points to image/compressed data */
} TILE_DATA;
#define TILE_DATA_DEFINED  
#endif

typedef struct {
  HUFFMAN_TABLE huff[8];
  int luminanceQuantization[64];
  int chrominanceQuantization[64];
  int tbl3_Quantization[64];
  int tbl4_Quantization[64];
  int threshLuminanceQuantization[64];
  int threshChrominanceQuantization[64];
  int tbl3_threshQuantization[64];
  int tbl4_threshQuantization[64];
  int en_last_dc[4];
}  JPEG_STRUCT;

/* 
* It creates the JPEG header bitstream.
*/
JPEGEXPORT
int JPEGEncodeTileInit(unsigned char *data,   /* points to the input tile */
int width, int height,
int bytes,                /* number of bytes per pixel */
int quality_factor,
int *Hsamp,int *Vsamp,  /* Hor. & Vert. subsampling factors */
int iflag,          /* 1/0 = Interleaved/Non-interleaved data */
TILE_DATA *tile_data,
int nu_huff,        /* # of Huffman Tables (a max of 8 tables, ie four DC-AC sets) */
JPEGHuffTable *huffman_table,  /* Huffman Tables in the Bits-Value format */
unsigned char *CompDCHuffIdent,
unsigned char *CompACHuffIdent,
int nu_qtables,       /* # of Q-tables (a max of 4 tables) */
JPEGQuantTable *q_table,
unsigned char *CompQuantIdent,
JPEG_STRUCT *jpeg_struct,
unsigned char *header_buffer,
long header_buffer_size,
long *header_bytes);

JPEGEXPORT
int JPEGEncodeTile(TILE_DATA *tile_data,
JPEG_STRUCT *jpeg_struct,
unsigned char *CompDCHuffIdent,
unsigned char *CompACHuffIdent,
unsigned char *CompQuantIdent,
unsigned char *out_buf,
long int out_buf_size,
long int *compr_size);

JPEGEXPORT
void Scale_Matrix(int numerator, 
int denominator, 
int *in, 
int *out);

JPEGEXPORT
void Clear_Last_DC(JPEG_STRUCT *jpeg_struct);

JPEGEXPORT
void EN_Encode_DC(int dc_val, 
int comp, 
HUFFMAN_TABLE *huffman, 
JPEG_STRUCT *jpeg_struct);

void EN_Encode_Block(int *block,
int comp, /* image component number */
HUFFMAN_TABLE *dcHuffman,
HUFFMAN_TABLE *acHuffman,
int *quant,
JPEG_STRUCT *jpeg_struct);

JPEGEXPORT
int EN_Encode_Scan_Gray(unsigned char *buf, 
int width, 
int height, 
JPEG_STRUCT *jpeg_struct);

JPEGEXPORT
int EN_Encode_Scan_Color11(unsigned char *buf,
int width,
int height,
int interleaved,
JPEG_STRUCT *jpeg_struct);

JPEGEXPORT
int EN_Encode_Scan_Color111(unsigned char *buf,
int width,
int height,
int interleaved, 
JPEG_STRUCT *jpeg_struct);

JPEGEXPORT
int EN_Encode_Scan_Color1111(unsigned char *buf,
int width, int height,
int interleaved, 
JPEG_STRUCT *jpeg_struct);

JPEGEXPORT
int EN_Encode_Scan_Color4114(unsigned char *buf,
int width, int height,
int interleaved, 
JPEG_STRUCT *jpeg_struct);

JPEGEXPORT
int EN_Encode_Scan_Color411(unsigned char *buf,
int width, int height,
int interleaved,
JPEG_STRUCT *jpeg_struct);

JPEGEXPORT
int EN_Encode_Scan_Color422(unsigned char *buf,
int width, int height,
int interleaved,
JPEG_STRUCT *jpeg_struct);

JPEGEXPORT
int EN_Encode_Scan_Color4224(unsigned char *buf,
int width, int height,
int interleaved,
JPEG_STRUCT *jpeg_struct);

JPEGEXPORT
int EN_Encode_Scan_Color(unsigned char *buf,
int width, int height,
JPEG_STRUCT *jpeg_struct);

JPEGEXPORT
int EN_Encode_Scan(TILE_DATA *tile_data,
JPEG_STRUCT *jpeg_struct);
