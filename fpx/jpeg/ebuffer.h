/* -*-C-*-
******************************************************************************
*
* File:         ebuffer.h
* RCS:          $Header$
* Description:  see ebuffer.c
* Author:       Gregory S. Yovanof
* Created:      Tue Jul 11 11:04:22 1995
* Initial Source Release:  Wed Jan 17 1996
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
void EB_Init(unsigned char *buf, /* output buffer */
   long int buf_size);
JPEGEXPORT
void EB_Clear(unsigned char *buf, /* output buffer */
   long int buf_size);
JPEGEXPORT
void EB_Write_Bits_Init(unsigned char *buf, /* compressed bitstream buffer */
   long int buf_size);
JPEGEXPORT
void EB_Write_Bytes(unsigned char *data,
   int size);
JPEGEXPORT
long cEB_Byte_Count(void);

JPEGEXPORT
void EB_Copy_To_Memory(unsigned char *buf,
   unsigned char *mem,
   long int num);
JPEGEXPORT
void EB_End(long int *bytes);

JPEGEXPORT
void EB_Write_Bits_End(long int *bytes);

JPEGEXPORT
int EB_Write_Bits(int val,
int nbits);
