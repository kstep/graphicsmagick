/* -*-C-*-
******************************************************************************
*
* File:         dparser.h
* RCS:          $Header$
* Description:  see dparser.c
* Author:       Hugh P. Nguyen
* Created:      Fri Dec  4 11:04:22 1992
* Modified:     Fri Dec  4 11:04:53 1992
* Language:     C
* Package:      Hewlett-Packard JPEG Encoder/Decoder
*
* Copyright (c) 1999 Digital Imaging Group, Inc.
* For conditions of distribution and use, see copyright notice
* in Flashpix.h
*
******************************************************************************
*/
#include "decoder.h"  

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

typedef struct t_list {
  void *table;                /* points to a DHUFF_TABLE or QUANT_TABLE */
  struct t_list *next;        /* points to next member of the list */
} TABLE_LIST;


typedef struct {
  int comp;                   /* component #, index to FRAME_COMPONENT */
  int hsampling;              /* horizontal sampling factor =
                                         frame.comps[comp].hsampling */
  int vsampling;              /* vertical sampling factor =
                                         frame.comps[comp].vsampling */
  DHUFF_TABLE *dc_table;    /* DC Huffman Table */
  DHUFF_TABLE *ac_table;    /* AC Huffman Table */
  QUANT_TABLE   *quant_table; /* Quantization Table */
} SCAN_COMPONENT;

typedef struct {                /* Structure for each scan in JPEG file */
  int ncomps;                 /* number of image components in scan */
  int gray_scale;             /* if TRUE, decode only Y channel */
  int start_spec;             /* start of spectral or predictor sel */
  int end_spec;               /* end of spectral selection */
  int approx_high;            /* succ. approximation bit pos. high */
  int approx_low;             /* succ. approximation bit pos. low */
  unsigned int restart_interv;/* restart interval, 0 if disabled */
  SCAN_COMPONENT *comps;      /* array of 'ncomps' items */
} SCAN;

JPEGEXPORT
int            DP_Skip(DB_STATE *db_state);
JPEGEXPORT
int            DP_Get_Next_Marker(DB_STATE *db_state);
JPEGEXPORT
unsigned char *DP_Parse_APP(DB_STATE *db_state, 
unsigned int *length, 
int *error_code);

JPEGEXPORT
long           DP_Parse_DRI(DB_STATE *db_state, 
int *error_code);

JPEGEXPORT
FRAME         *DP_Parse_SOF(DB_STATE *db_state, 
int *error_code);

JPEGEXPORT
TABLE_LIST    *DP_Parse_DQT(DB_STATE *db_state, 
int *ntables, 
int type, 
int *error_code);

JPEGEXPORT
TABLE_LIST    *DP_Parse_DHT(DB_STATE *db_state, 
int *ntables, 
int *error_code);

JPEGEXPORT
SCAN          *DP_Parse_SOS(DB_STATE *db_state,
FRAME *frame,
DHUFF_TABLE *dc_huffman_tables[],
DHUFF_TABLE *ac_huffman_tables[],
QUANT_TABLE *quant_tables[],
int *error_code
);

JPEGEXPORT
void           DP_Free_Frame(FRAME *frame);
JPEGEXPORT
void           DP_Free_Scan(SCAN *scan);
JPEGEXPORT
void           DP_Free_Table_Links(TABLE_LIST *tlist);
JPEGEXPORT
int            Decode_Scan(
DB_STATE *db_state,
FRAME *frame,
SCAN *scan,
int method,      /* 0 = Chen, 1 = Winograd, 2 = Pruned Winograd */
int interleave
);
