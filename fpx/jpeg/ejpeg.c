/* -*-C-*-
******************************************************************************
*
* File:         ejpeg.c
* RCS:          $Header$
* Description:  main module of JPEG encoder library.
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
#include "ebuffer.h"
#include "ejpeg.h"

unsigned char *image_data;   /* used by Read_Next_Rows_From_Memory */
int nrows_left;              /* used by Read_Next_Rows_From_Memory */
FILE *ej_infp;               /* used by Read_Next_Rows_From_File */
FILE *ej_outfp;              /* used by Write_Bytes_To_File */

/********************************** INPUT  **********************************/

int Read_Next_Rows_From_Memory(unsigned char *buf, int nrows, int nbytes_per_row)
{
  int i, j;

  /* Check if there are enough data in the buffer 'image_data' */
  if (nrows_left < nrows) {
    nrows = (nrows_left <= 0) ? 0 : nrows_left;
  }
  for (i = 0; i < nrows; i++) {
    for (j = nbytes_per_row; j > 0; j--) *buf++ = *image_data++;
  }
  nrows_left -= nrows;
  return(nrows);
}

int Read_Next_Rows_From_File(unsigned char *buf, int nrows, int nbytes_per_row)
{
  return(fread(buf, nbytes_per_row, nrows, ej_infp));
}

/********************************** OUTPUT **********************************/

void Write_Bytes_To_File(unsigned char *data, int nbytes)
{
  fwrite(data, nbytes, 1, ej_outfp);
}
