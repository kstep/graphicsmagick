/* -*-C-*-
*************************************************************************
*
* File:         ebuffer.c
* RCS:          $Header$
* Description:  performs I/O
* Author:       Gregory S. Yovanof
* Created:      Tue Jul 11 11:04:22 1995
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
#include <string.h>
#include "jpegconf.h"
#include "ebuffer.h"

/****************************** GLOBAL VARIABLES **************************/
unsigned char *eb_ptr;      /* points to next avaible byte in output buffer */
unsigned char eb_byte;      /* current output byte to be inserted into buffer */
int eb_nbits;               /* # bits available in *eb_ptr */
unsigned char *eb_end_ptr, *eb_start_ptr;
long int eb_byte_count;

void EB_Init(unsigned char *buf, /* output buffer */
long int buf_size)
{
  eb_ptr = eb_start_ptr = buf;
  eb_end_ptr = buf+buf_size;
  eb_byte = 0;
  eb_nbits = 8;
  eb_byte_count = 0;
}

void EB_Clear(unsigned char *buf, /* output buffer */
long int buf_size)
{
  unsigned char *eb_ptr, *eb_end_ptr;

  eb_end_ptr = buf + buf_size;
  for (eb_ptr = buf; eb_ptr < eb_end_ptr ;*eb_ptr++ = 0)
    eb_byte = 0;
  eb_nbits = 8;
  eb_byte_count = 0;
}

void EB_Write_Bits_Init(unsigned char *buf, /* compressed bitstream buffer */
long int buf_size)
{
  eb_ptr = eb_start_ptr = buf;
  eb_end_ptr = buf + buf_size;
  eb_byte = 0;
  eb_nbits = 8;
  eb_byte_count = 0;
}

void EB_Write_Bytes(unsigned char *data,
int size)
{
  int i;
  unsigned char *ptr;

  /* byte-align previous bits if any */
  if (eb_nbits < 8) {
    *eb_ptr++ = eb_byte; 
    eb_byte_count++;
    if (eb_byte == 0xff) *eb_ptr++ = 0x00; /* byte stuffing */
  }
  for (i=0, ptr=data; i < size ;i++) {
    *eb_ptr++ = *ptr++;
  }
}

/* calculates the actual number of bytes written into output buffer */
long cEB_Byte_Count(void)
{
  return((long)(eb_ptr - eb_start_ptr));
}

void EB_Copy_To_Memory(unsigned char *buf,
unsigned char *mem,
long int num)
{
  long int i;
  unsigned char *mem_ptr,*buf_ptr;

  for(i=0,mem_ptr=mem,buf_ptr=buf; i < num ;*mem_ptr++=*buf_ptr++);

}

/* calculates the actual number of bytes written into output buffer */
void EB_End(long int *bytes)
{
  *bytes = (long)(eb_ptr - eb_start_ptr);
}

/* calculates the actual number of bytes written into output buffer */
void EB_Write_Bits_End(long int *bytes)
{
  *bytes = eb_ptr - eb_start_ptr;
}

/*
 * Write 'nbits' bits of 'val' to the output buffer.
 * Return 0 if successful, -1 if out-of-memory.
 * 
 */
int EB_Write_Bits(int val,
int nbits)
{
  while (nbits > eb_nbits) {
    eb_byte_count++;
    /* extract eb_nbits from val and insert into current byte */
    nbits -= eb_nbits;
    if ((*eb_ptr++ = eb_byte | ((val >> nbits) & ((1 << eb_nbits)-1)))
        == 0xff) {
      *eb_ptr++ = 0x00;    /* Stuff byte */
      eb_byte_count++;
    }
    eb_byte = 0;
    eb_nbits = 8;
    if (eb_ptr >= eb_end_ptr) {
#ifdef UNIX
      fprintf(stderr,
          "\n warning: insufficient buffer capacity (eb_ptr=%ld eb_byte_count=%d)\n",
          eb_ptr,eb_byte_count);
#endif /* UNIX */
      return(-1);
    }
  }
  /* Now insert nbits (nbits <= eb_nbits) of val into current byte */
  eb_nbits -= nbits;
  eb_byte |= (val & ((1 << nbits)-1)) << eb_nbits;
  return(0);
}
