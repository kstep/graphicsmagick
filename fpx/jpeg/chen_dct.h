/* -*-C-*-
******************************************************************************
*
* File:         chen_dct.h
* RCS:          $Header$
* Description:  see chen_dct.c
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
void IDct_Chen(DB_STATE *db_state,
int *x);

JPEGEXPORT
void Fill_Chen_Quant_Table(int *in,   /* regular quantization table */
int *out);  /* chen quantization table */
