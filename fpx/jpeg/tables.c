/* -*-C-*-
******************************************************************************
*
* File:         tables.c
* RCS:          $Header$
* Description:  Sets Huffman & Quantization Tables to the JPEG recommended ones.
* Author:       Gregory Yovanof
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
#define IMP_ENCODER
#include "jpegconf.h"
#include "enctile.h"
#include "encoder.h"
#include "public.h"
#include "tables.h"

int
SetDefaultTables(void *encoder)
{
   ENCODER_STRUCT *jpg = (ENCODER_STRUCT *) encoder;
   JPEGHuffTable HuffTables[8];
   JPEGQuantTable QuantTables[4];
   unsigned char   CompDCHuffIdent[4];
   unsigned char   CompACHuffIdent[4];
   unsigned char   CompQuantIdent[4]; 
   int err_num;
   
   HuffTables[0].bits = DefaultLuminanceDCBits;
   HuffTables[0].vals = DefaultLuminanceDCValues;
   HuffTables[0].hclass = 0;
   HuffTables[0].ident = 0;
   HuffTables[1].bits = DefaultLuminanceACBits;
   HuffTables[1].vals = DefaultLuminanceACValues;
   HuffTables[1].hclass = 1;
   HuffTables[1].ident = 0;
   HuffTables[2].bits = DefaultChrominanceDCBits;
   HuffTables[2].vals = DefaultChrominanceDCValues;
   HuffTables[2].hclass = 0;
   HuffTables[2].ident = 1;
   HuffTables[3].bits = DefaultChrominanceACBits;
   HuffTables[3].vals = DefaultChrominanceACValues;
   HuffTables[3].hclass = 1;
   HuffTables[3].ident = 1;

   CompACHuffIdent[0]=CompDCHuffIdent[0]=0;
   CompACHuffIdent[1]=CompDCHuffIdent[1]=1;
   CompACHuffIdent[2]=CompDCHuffIdent[2]=1;
   CompACHuffIdent[3]=CompDCHuffIdent[3]=1;

   err_num = eJPEG_SetHuffTables(jpg,4,(JPEGHuffTable **)(&HuffTables),
         CompDCHuffIdent,CompACHuffIdent);
   if (err_num != eJPEG_NO_ERROR)
  return(err_num);

   QuantTables[0].quantizer = DefaultLumQuant;
   QuantTables[0].ident = 0;
   QuantTables[1].quantizer = DefaultChromQuant;
   QuantTables[1].ident = 1;

   CompQuantIdent[0] = 0; 
   CompQuantIdent[1] = 1; 
   CompQuantIdent[2] = 1; 
   CompQuantIdent[3] = 1; 

   err_num = eJPEG_SetQuantTables(jpg,2,(JPEGQuantTable **)(&QuantTables),CompQuantIdent);
   if (err_num != eJPEG_NO_ERROR)
  return(err_num);

   return(eJPEG_NO_ERROR);

}
