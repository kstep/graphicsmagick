/* -*-C-*-
******************************************************************************
*
* File:         eparser.c
* RCS:          $Header$
* Description:  writes JPEG MARKERs
* Author:       Hugh P. Nguyen
* Created:      Fri Dec  4 11:04:22 1992
* Modified:     Mon Jan 29 16:05:22 1996 (Gregory S. Yovanof)
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
#include <stdlib.h>
#include <stdio.h>
#include "jpegconf.h"
#include "ebuffer.h"
#include "eparser.h"
#include "fpxmem.h"

#define EP_BUFFER_SIZE 256
unsigned char *ep_buf;

/********************************** MACROS **********************************/

#define ASSIGN_MARKER(marker, ptr)\
    *ptr++ = 0xff;\
    *ptr++ = marker

#define ASSIGN_INT(value, ptr)\
    *ptr++ = (value >> 8) & 0xff;\
    *ptr++ = value & 0xff

/***************************  exported functions ****************************/
/*
 * Return 0 if successful, -1 otherwise
 */
int EP_Begin(void)                /* number of image components */
{
  if ((ep_buf = (unsigned char *) FPX_malloc(EP_BUFFER_SIZE)) == NULL) {
    return(-1);
  } else {
    return(0);
  }
}

void EP_End(void)
{
  if (ep_buf != NULL) {
    FPX_free(ep_buf);
    ep_buf = NULL;
  }
}

void EP_Write_SOI(void)
{
  unsigned char *ptr;
  ptr = ep_buf;
  ASSIGN_MARKER(MARKER_SOI, ptr);
  EB_Write_Bytes(ep_buf, 2);
}

void EP_Write_EOI(void)
{
  unsigned char *ptr;
  ptr = ep_buf;
  ASSIGN_MARKER(MARKER_EOI, ptr);
  EB_Write_Bytes(ep_buf, 2);
}

/*
 * Return 0 if successful, -1 otherwise
 */
int EP_Write_SOF(int width,
int height,
int *Hsamp,
int *Vsamp,
int ncomps,
unsigned char *CompQuantIdent)
{
  unsigned char *ptr;
  int len, i;

  len = 8 + ncomps*3;
  if ((len+2) > EP_BUFFER_SIZE) {
    return(-1);
  }

  ptr = ep_buf;
  ASSIGN_MARKER(MARKER_SOF0, ptr);
  ASSIGN_INT(len, ptr);
  *ptr++ = 8;      /* Sample Precision */
  ASSIGN_INT(height, ptr);
  ASSIGN_INT(width , ptr);
  *ptr++ = ncomps;

  if (ncomps == 1) {
    *ptr++ = 0;    /* Component identifier */
    *ptr++ = 0x11; /* h0, v0 sampling */
    *ptr++ = CompQuantIdent[0];    /* quantization table selector */
  } else {
    *ptr++ = 1;
    *ptr++ = (Hsamp[0] << 4) + Vsamp[0];
    *ptr++ = CompQuantIdent[0];
    for (i = 2; i <= ncomps; i++) {
      *ptr++ = i;
      *ptr++ = (Hsamp[i-1] << 4) + Vsamp[i-1];
      *ptr++ = CompQuantIdent[i-1];
    }
  }

  EB_Write_Bytes(ep_buf, len+2);
  return(0);
}

void EP_Write_DQT(int precision,        /* 0 = 8-bit, 1 = 16-bit */
int ident,
int elements[64])
{
  unsigned char *ptr;
  int len, i;

  len = 67;
  ptr = ep_buf;
  ASSIGN_MARKER(MARKER_DQT, ptr);
  ASSIGN_INT(len, ptr);
  *ptr++ = (precision << 4) + ident;
  for (i = 64; i > 0; i--) *ptr++ = *elements++;

  EB_Write_Bytes(ep_buf, len+2);
}

/*
 * Return 0 if successful, -1 otherwise
 */
int EP_Write_DHTs(int ntables,            /* number of tables */
unsigned char clas[],            /* 0 = DC or lossless table, 1 = AC table */
unsigned char ident[],
unsigned char *bits[],
unsigned char *huffval[])
{
  unsigned char *ptr;
  int len, nvals, i, j;

  for (nvals = i = 0; i < ntables; i++) {
    for (j = 0; j < 16; j++) nvals += bits[i][j];
  }

  len = 2 + 17*ntables + nvals;
  ptr = ep_buf;
  ASSIGN_MARKER(MARKER_DHT, ptr);
  ASSIGN_INT(len, ptr);
  EB_Write_Bytes(ep_buf, 4);

  for (i = 0; i < ntables; i++) {
    for (nvals = j = 0; j < 16; j++) nvals += bits[i][j];

    len = 17 + nvals;
    if (len > EP_BUFFER_SIZE) {
      return(-1);
    }

    ptr = ep_buf;
    *ptr++ = (clas[i] << 4) + ident[i];
    for (j = 0; j < 16; j++) {
      *ptr++ = bits[i][j];
    }
    for (j = 0; j < nvals; j++) {
      *ptr++ = huffval[i][j];
    }

    EB_Write_Bytes(ep_buf, len);
  }
  return(0);
}

/*
 * Return 0 if successful, -1 otherwise
 */
int EP_Write_SOS(int ncomps, 
unsigned char *CompDCHuffIdent,
unsigned char *CompACHuffIdent)
{
  unsigned char *ptr;
  unsigned char val;
  int len, i;

  len = 6 + 2*ncomps;
  if ((len+2) > EP_BUFFER_SIZE) {
    return(-1);
  }

  ptr = ep_buf;
  ASSIGN_MARKER(MARKER_SOS, ptr);
  ASSIGN_INT(len, ptr);
  *ptr++ = ncomps;
  if (ncomps == 1) {
    *ptr++ = 0;
    *ptr++ = 0x00;
  } else {
#ifdef DEBUG
    if ((ncomps != 1) && (ncomps ! = 2) && (ncomps != 3) && (ncomps != 4))
      debug("ncomps must be 1, 2, 3 or 4 in EP_Write_SOS");
#endif
    *ptr++ = 1;    /* Scan Component Selector */
    val = (CompDCHuffIdent[0] << 4) + CompACHuffIdent[0];
    *ptr++ = val; /* DC-AC entropy coding table selector */
    for (i = 2; i <= ncomps; i++) {
      *ptr++ = i;
      val = (CompDCHuffIdent[i-1] << 4) + CompACHuffIdent[i-1];
      *ptr++ = val;
    }
  }
  *ptr++ = 0;  /* Start of spectral selection */
  *ptr++ = 63; /* End of spectral selection */
  *ptr++ = 0;  /* Successive approximation bit position (High-Low) */

  EB_Write_Bytes(ep_buf, len+2);
  return(0);
}
