/* -*-C-*-
******************************************************************************
*
* File:         eparser.h
* RCS:          $Header$
* Description:  see eparser.c
* Author:       Hugh P. Nguyen
* Created:      Fri Dec  4 11:04:22 1992
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

#ifndef MARKER_NONE

#define MARKER_NONE 0x00
#define MARKER_END_FILE 0x100

#define MARKER_SOF0 0xc0
#define MARKER_SOF1 0xc1
#define MARKER_SOF2 0xc2
#define MARKER_SOF3 0xc3
#define MARKER_SOF5 0xc5
#define MARKER_SOF6 0xc6
#define MARKER_SOF7 0xc7
#define MARKER_SOF8 0xc8
#define MARKER_SOF9 0xc9
#define MARKER_SOFA 0xca
#define MARKER_SOFB 0xcb
#define MARKER_SOFD 0xcd
#define MARKER_SOFE 0xce
#define MARKER_SOFF 0xcf

#define MARKER_DHT  0xc4
#define MARKER_DAC  0xcc

#define MARKER_RST0 0xd0
#define MARKER_RST1 0xd1
#define MARKER_RST2 0xd2
#define MARKER_RST3 0xd3
#define MARKER_RST4 0xd4
#define MARKER_RST5 0xd5
#define MARKER_RST6 0xd6
#define MARKER_RST7 0xd7

#define MARKER_SOI  0xd8
#define MARKER_EOI  0xd9
#define MARKER_SOS  0xda
#define MARKER_DQT  0xdb
#define MARKER_DNL  0xdc
#define MARKER_DRI  0xdd
#define MARKER_DHP  0xde
#define MARKER_EXP  0xdf

#define MARKER_APP  0xe0        /* from 0xe0 - 0xef */
#define MARKER_JPG  0xf0        /* from 0xf0 - 0xfd */
#define MARKER_COM  0xfe

#endif

/* This macro will map 0xe0-0xef to APP, 0xf0-0xfd to JPG */
#define CONVERT_TO_MARKER(x)\
    (((x) >= 0xe0) && ((x) <= 0xfd)) ? (x) & 0xf0 : (x)

JPEGEXPORT
int EP_Begin(void);
JPEGEXPORT
void EP_End(void);

JPEGEXPORT
void EP_Write_SOI(void);

JPEGEXPORT
void EP_Write_EOI(void);

JPEGEXPORT
int EP_Write_SOF(int width,
int height,
int *Hsamp,
int *Vsamp,
int ncomps,
unsigned char *CompQuantIdent);

JPEGEXPORT
void EP_Write_DQT(int precision,        /* 0 = 8-bit, 1 = 16-bit */
int ident,
int elements[64]);

JPEGEXPORT
int EP_Write_DHTs(int ntables,            /* number of tables */
unsigned char clas[],            /* 0 = DC or lossless table, 1 = AC table */
unsigned char ident[],
unsigned char *bits[],
unsigned char *huffval[]);

JPEGEXPORT
int EP_Write_SOS(int ncomps,
unsigned char *CompDCHuffIdent,
unsigned char *CompACHuffIdent);
