/* -*-C-*-
******************************************************************************
*
* File:         dhuff.c
* RCS:          $Header$
* Description:  builds Huffman tree, and performs Huffman decoding
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

#include <stdlib.h>
#include <stdio.h>
#include "jpegconf.h"
#include "decoder.h"
#include "dbuffer.h"
#include "dectile.h"
#include "djpeg.h"
#include "dparser.h"
#include "fpxmem.h"
/**/

void Assign_Code(
unsigned int code,          /* code word */
int val,                    /* Huffman value - symbol */
int nbits,                  /* number of bits in code word */
HUFFMAN_ELEM *huffelem)     /* Huffman table to receive the assignment */
{
    int i, bound;
    
    bound = (code + 1) << (8 - nbits);
    for (i = (code << (8 - nbits)); i < bound; i++) {
    huffelem[i].codelen = (unsigned char) nbits;
    huffelem[i].value   = (unsigned char) val;
        huffelem[i].hufftree= NULL;
    }
}

void Assign_Tree(
unsigned int start_code,    /* starting code */
int ncodes,                 /* total number of codes to be assigned */
int start_index,            /* index to huffval = value for start_code */
int nbits,                  /* number of bits in code words */
HUFFMAN_ELEM *huffelem)     /* Huffman table to receive the assignment */
{
    int first, last, i, index, mask;
    HUFFMAN_TREE *hufftree;
    
    index = nbits - 9;
    first = (int) (start_code >> (index + 1));
    last  = (start_code + ncodes - 1) >> (index + 1);
    if (first > 255) return;
    if (last  > 255) last  = 255;
    mask  = (1 << (index+1)) - 1;
    
    if (first == last) {
        hufftree = huffelem[first].hufftree;
        hufftree->mincode[index] = start_code & mask;
        hufftree->maxcode[index] = (start_code+ncodes-1) & mask;
        hufftree->valptr [index] = start_index - hufftree->mincode[index];
    } else {
        hufftree = huffelem[first].hufftree;
        hufftree->mincode[index] = start_code & mask;
        hufftree->maxcode[index] = mask;
        hufftree->valptr [index] = start_index - hufftree->mincode[index];

        start_index += ((first+1) << (index+1)) - start_code;
        for (i = first+1; i < last; i++) {
            hufftree = huffelem[i].hufftree;
            hufftree->mincode[index] = 0;
            hufftree->maxcode[index] = mask;
            hufftree->valptr [index] = start_index;
            start_index += mask+1;
        }
        
        hufftree = huffelem[last].hufftree;
        hufftree->mincode[index] = 0;
        hufftree->maxcode[index] = (start_code+ncodes-1) & mask;
        hufftree->valptr [index] = start_index;
    }
}

DHUFF_TABLE *Build_Huffman_Table(
int huff_class,
int ident,
unsigned char *bits,        /* bit-length frequency (indexed on length) */
unsigned char *huffval)     /* huffman values - see JPEG doc */
{
    DHUFF_TABLE *huff_table;
    HUFFMAN_ELEM  *huff_elem;
    HUFFMAN_TREE  *huff_tree;
    int *huff_val;
    int bitlen, j, k, freq, ptr;
    unsigned int code;
    
    if ((huff_table =(DHUFF_TABLE *) FPX_malloc(sizeof(DHUFF_TABLE)))==NULL) {
        return(NULL);
    }
    huff_table->huff_class = huff_class; 
    huff_table->ident = ident;
    huff_elem = huff_table->huffelem;
    huff_val  = huff_table->huffval;

    /* Fill huff_table */
    code = 0;
    for (bitlen = 1; bitlen <= 8; bitlen++) {   /* Build primary table */
    freq = *bits++;
    if (freq) {
            for (j = 0; j < freq; j++) {
                Assign_Code(code++, *huffval++, bitlen, huff_table->huffelem);
            }
        }
        code <<= 1;
    }
    
    code >>= 1;
    huff_elem = huff_table->huffelem;
    for (j = 0; j < (int)code; j++) huff_elem[j].hufftree = NULL;
    for (j = code; j < 256; j++) {
        huff_elem[j].codelen = 0;
    /*      huff_elem[j].value   = -1; */
        if ((huff_tree =(HUFFMAN_TREE *)FPX_malloc(sizeof(HUFFMAN_TREE)))==NULL) {
            for (k = code; k < j; k++) {
              if(huff_elem[k].hufftree) {   /* CHG_JPEG_MEM_FIX - added check & clear */
                  FPX_free(huff_elem[k].hufftree);
                  huff_elem[k].hufftree = NULL;
                }
            }
            FPX_free(huff_table);
            return(NULL);
        } else {
            huff_elem[j].hufftree = huff_tree;
            for (k = 0; k < 8; k++) {
                huff_elem[j].hufftree->mincode[k] =
                huff_elem[j].hufftree->maxcode[k] = -1;
            }
        }
    }

    code <<= 1;
    ptr = 0;
    for (bitlen = 9; bitlen <= 16; bitlen++) {   /* Build Huffman trees */
    freq = *bits++;
    if (freq) {
            for (j = 0; j < freq; j++) {
                *huff_val++ = *huffval++;
            }
            Assign_Tree(code, freq, ptr, bitlen, huff_elem);
            code += freq;
            ptr  += freq;
        }
        code <<= 1;
    }
    return(huff_table);
}


int Decode_Huffman(
DB_STATE *db_state,
DHUFF_TABLE *huff_table)
{
    register HUFFMAN_ELEM  *huff_elem;
    register int n, code, *p;
    HUFFMAN_TREE  *huff_tree;
    
    huff_elem = huff_table->huffelem + DB_Get_Byte(db_state);
  n = huff_elem->codelen;
  if (n) {
        PUSH_BACK_BITS(8-n);
        return(huff_elem->value);
    } else {            /* decode bit by bit using huffman tree */
        huff_tree = huff_elem->hufftree;
#ifdef DEBUG
        if (huff_tree == NULL) {
            debug("Huffman Tree is NULL");
            return(0);
        }
#endif
        code = DB_Get_Byte(db_state);
        for (p = huff_tree->maxcode, n = 7; (n >= 0) &&
            ((code >> n) > *p++); n--);
        if (n >= 0) {     /* code > maxcode */
            PUSH_BACK_BITS(n);
            return(huff_table->huffval[(code >> n) + huff_tree->valptr[7-n]]);
        } else {            /* fail to decode - Return 0 (autom. resync) */
#ifdef WARNING
            warning(WARNING_DATA);
#endif
            return(0);
        }
    }
}
