/* -*-C-*-
******************************************************************************
*
* File:         dhuff.h
* RCS:          $Header$
* Description:  see dhuff.c
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

JPEGEXPORT
int Decode_Huffman(DB_STATE *db_state,
           DHUFF_TABLE *huff_table);

JPEGEXPORT
DHUFF_TABLE *Build_Huffman_Table(
  int huff_class,
  int ident,
  unsigned char *bits,        /* bit-length frequency (indexed on length) */
  unsigned char *huffval);     /* huffman values - see JPEG doc */
