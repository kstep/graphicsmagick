/* -*-C-*-
******************************************************************************
*
* File:         huffman.c     
* RCS:          $Header$
* Description:  builds Huffman encoding table
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

#include <stdio.h>
#include "jpegconf.h"
#include "enctile.h"
#include "eparser.h"
#include "huffman.h"

void BuildHuffmanTable(unsigned char *bits,
unsigned char *huffval,
HUFFMAN_TABLE *table)
{
  int i, j, k, lastk, code, si;
  int huffsize[257];
  int huffcode[257];

  /* Init table */
  for (i = 0; i < 256; i++) {
    table->ehufcode[i] = 0;
    table->ehufsize[i] = 0;
  }

  /* Generate Size Table -- see JPEG spec. */
  /* Note that list BITS in JPEG document has index from 1-16, but this list
       'bits' has index from 0-15, thus BITS[i] is replaced by bits[i-1] */
  for (k=0, i=1; i < 17; i++) {
    for(j=1; j <= bits[i-1]; j++) {
      huffsize[k++] = i;
    }
  }
  huffsize[k] = 0;
  lastk = k;

  /* Generate Code Table -- see JPEG document. */
  k = 0;
  code = 0;
  si = huffsize[0];
  while (1) {
    do {
      huffcode[k++] = code++;
    } while((huffsize[k]==si)&&(k<257));   /* Overflow Detection */
    if (!huffsize[k]) {                    /* All finished */
      break;
    }
    do {              /* Shift next code to expand prefix. */
      code <<= 1;
      si++;
    } while(huffsize[k] != si);
  }

  /* Order Codes -- see JPEG document. */
  for(k = 0; k < lastk; k++) {
    i = (int)huffval[k];
    table->ehufcode[i] = huffcode[k];
    table->ehufsize[i] = huffsize[k];
  }
}
