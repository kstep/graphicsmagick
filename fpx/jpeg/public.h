/* -*-C-*-
******************************************************************************
*
* File:         public.h
* Description:  A semi-private structure for the "public.c" routines,
*               separated just in case somebody wants to share it.
* Author:       Kirt Winter
* Created:      Fri Mar  1 11:21:06 1995
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
#ifndef JPEGEXPORT
#include "jpegconf.h"
#endif

typedef struct {
  unsigned char subsampling;  /* stored in FPX tile header format */
    int     iHsamp[4];  /* same thing, but in the format the 
           encoder prefers */
    int     iVsamp[4];    
    JPEG_STRUCT jpegStruct; /* used by the encoder to store tables */
    unsigned char ssDisabled; /* if non-zero, internal subsampling is
           disabled */
    unsigned char YCrCbDisabled;  /* if non-zero, internal YCrCb is disabled */
    int   xPixels;  /* x dimension of a tile in pixels */
    int   yPixels;  /* y dimension of a tile in pixels */
    int   bytes;    /* number of bytes per pixel */
    int   qFactor;  /* the quality level of the encoding */
    int   interleaveType; /* 0 =interleaved channels, 1 = non-interleaved */
    TILE_DATA tile_data;
    int   nu_huff;  /* # of Huffman Tables (8, four DC-AC sets) */
    int   nu_qtables; /* # of Q-tables (a max of 4 tables) */
    unsigned char  *scratch;  /* place to hold rotated/subsampled data */
    unsigned char  *header; /* place to hold the JPEG header */
    long  headerBytes;  /* how big is the header anyway? */
    JPEGHuffTable  HuffTables[8]; /* Pointers to a maximum of four pairs of DC-AC 
                                      Huffman Tables */
    JPEGQuantTable QuantTables[4]; /* Pointers to a maximum of 4 Quantization Tables */
    unsigned char   CompDCHuffIdent[4];  /* CompHuffIdent[i] = k, associates channel i with
                                          the DC Huffman whose Ident number is k */
    unsigned char   CompACHuffIdent[4];  /* CompHuffIdent[i] = k, associates channel i with
                                          the AC Huffman whose Ident number is k */
    unsigned char   CompQuantIdent[4]; /* CompQuantIdent[i] = k, associates channel i with
                                          the Quantization Table whose Ident number is k */
} ENCODER_STRUCT, *ENCODER;
