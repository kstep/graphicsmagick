/* -*-C-*-
******************************************************************************
*
* File:         dbuffer.c
* RCS:          $Header$
* Description:  I/O Buffer
* Author:       Hugh P. Nguyen
* Created:      Fri Dec  4 11:04:22 1992
* Modified:     Fri Apr 19 11:04:53 1996 by Gregory S. Yovanof
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
#include "decoder.h"
#include "dbuffer.h"
#include "dectile.h"
#include "djpeg.h"
#include "dparser.h"
#include "fpxmem.h"

#ifdef USE_ALL_TABLES
#define USE_SMALL_TABLES
#endif

#define BUFFER_SIZE  4096   /* local buffer size */
#define EXTRA_SIZE   256    /* original size for extra bytes after buffer */

/**************************** MACROS  & UTILS ******************************/

void Clear_Buffer(unsigned char *buf,
int size)
{
  for (; size > 0; size--) *buf++ = 0; 
}

#define FILL_BUFFER \
    *db_state->db_buf = *db_state->db_ptr;\
    if ((db_state->db_nbytes=(*proc_read_bytes)\
   (db_state, db_state->db_buf+1, db_state->db_size-1)) <= 0) {\
        Clear_Buffer(db_state->db_buf+1, db_state->db_size-1);\
        db_state->db_nbytes = db_state->db_size-1;\
        warning(WARNING_EOF);\
    };\
    db_state->db_ptr = db_state->db_buf

#define FILL_BUFFER2 \
    *db_state->db_buf = *db_state->db_ptr;\
    if ((db_state->db_nbytes=(*proc_read_bytes)\
   (db_state, (db_state->db_ptr = db_state->db_buf+1), db_state->db_size-1)) <=0) {\
        Clear_Buffer(db_state->db_ptr, db_state->db_size-1);\
        db_state->db_nbytes = db_state->db_size-1;\
        warning(WARNING_EOF);\
    }

#define UPDATE_BUFFER \
    if (--db_state->db_nbytes <= 0) {\
        *db_state->db_buf = *(db_state->db_ptr-1);\
        if ((db_state->db_nbytes=(*proc_read_bytes)\
      (db_state, (db_state->db_ptr=db_state->db_buf+1), db_state->db_size-1))==0) {\
            db_state->db_nbits = 0;\
        }\
    }

/*
 * Update db_state->db_nbytes and db_state->db_ptr to point to next availabe byte.  If
 * 0xff00 is detected, the byte 0x00 will be removed.
 */
#define GO_TO_NEXT_ALIGNED_BYTE \
    if (--db_state->db_nbytes <= 0) {\
        FILL_BUFFER;\
    }\
   (db_state->db_byte_count) += 1;\
    if (*db_state->db_ptr++ == 0xff) {\
        if (*db_state->db_ptr == 0x00) {\
            if (db_state->db_push_back_ff00) {\
                db_state->db_push_back_ff00 = 0;\
            } else {\
                *db_state->db_ptr = 0xff;\
                if (--db_state->db_nbytes <= 0) {\
                    FILL_BUFFER2;\
                } else {\
                    db_state->db_ptr++;\
           (db_state->db_byte_count) += 1;\
                }\
            }\
        }\
    }

/*
 * Same as GO_TO_NEXT_ALIGNED_BYTE, but fetch the byte into 'data'.
 */
#define GET_NEXT_ALIGNED_BYTE(data) \
    if (--db_state->db_nbytes <= 0) {\
        FILL_BUFFER;\
    }\
   (db_state->db_byte_count) +=1;\
    if (*db_state->db_ptr++ == 0xff) {\
        if ((data = *db_state->db_ptr) == 0x00) {\
            if (push_back_ff00) {\
                push_back_ff00 = 0;\
            } else {\
                *db_state->db_ptr = 0xff;\
                if (--db_state->db_nbytes <= 0) {\
                    FILL_BUFFER2;\
                    data = *db_state->db_ptr;\
                } else {\
                    data = *++db_state->db_ptr;\
                }\
            }\
        }\
    } else {\
        data = *db_state->db_ptr;\
    }


/*********************************** INPUT ********************************/

/* These functions will be used to perform I/O */
int  (*proc_read_bytes)(void *, void *, int);   /* will be called to read in n bytes */
void (*proc_write_rows)(void *, void *, int, int);   /* will be called to output n rows */

void DB_Init (DB_STATE *db_state,
              int  (*read_bytes)(void *, void *, int),
              void (*write_rows)(void *, void *, int, int),
              unsigned char *data_in,
              size_t data_in_size,
              unsigned char *data_out,
              int interleave,
              int which   /* 1: set read_bytes */
              /* 0,2: set read_bytes, write_rows & data */
)
{
  if (which == 1) {
    proc_read_bytes = read_bytes;
    db_state->db_inptr = db_state->db_inbuf = data_in;
    db_state->db_inbuf_size = (int) data_in_size;
    db_state->nbytes_left = (long) db_state->db_inbuf_size;
  }
  else { /* 0 or 2 */
    proc_read_bytes = read_bytes;
    db_state->db_inptr = db_state->db_inbuf = data_in;
    db_state->db_inbuf_size = (int) data_in_size;
    db_state->nbytes_left = (long) db_state->db_inbuf_size;
    proc_write_rows = write_rows;
    db_state->db_supplied_ptr = db_state->db_supplied_bufs;
    db_state->db_outptr = db_state->db_outbuf = data_out;
  }
}

/*
 * Return 0 if successful, -1 if memory can not be allocated.
 */
int DB_Begin(DB_STATE *db_state)
{
  /*
   * It is only executed during pass-1,2 in DE_Decode()
   */
  if (db_state->db_buf == NULL){
    if ((db_state->db_buf = (unsigned char *)FPX_malloc(BUFFER_SIZE+EXTRA_SIZE))==NULL) {
      return(-1);
    }
  }
  db_state->db_size = BUFFER_SIZE + EXTRA_SIZE;
  db_state->db_ptr = db_state->db_buf;
  db_state->db_nbytes = (*proc_read_bytes)(db_state, db_state->db_ptr, BUFFER_SIZE);
  db_state->db_nbits = 8;
  db_state->db_byte_count = 0;
  db_state->db_push_back_ff00 = 0;

  return(0);
  
}

void DB_End( DB_STATE *db_state )
{
  if (db_state->db_buf != NULL) FPX_free(db_state->db_buf);
  db_state->db_buf = NULL;
  db_state->db_ptr = NULL;
  db_state->db_nbytes = db_state->db_nbits = 0;
  db_state->db_inptr -= db_state->db_nbytes;
}

/*
 * Returns number of processed bytes 
*/
long DB_Processed_Bytes( DB_STATE *db_state )
{
  return(db_state->db_byte_count);
}

/*
 * Return the pointer to a buffer with next unprocessed 'nbytes' of data, or
 * NULL if request can not be satisfied.  If error, error_code = one of
 * ERROR_ types defined in djpeg.h.
 */
unsigned char *DB_Get_Data( DB_STATE *db_state,
unsigned int nbytes,
int *error_code)
{
  if ((int)nbytes > db_state->db_nbytes) {   /* current buffer does not have enough data */
    if ((int)nbytes > db_state->db_size) { /* buffer is not large enough */
      int size;

      size = db_state->db_ptr - db_state->db_buf;
      if ((db_state->db_buf =(unsigned char *) 
           realloc(db_state->db_buf, nbytes)) == NULL) {
        *error_code = ERROR_MEM;
        return(NULL);
      }
      db_state->db_ptr = db_state->db_buf + size;
    }
      
    /* Now it is guaranteed that the buffer is at least 'nbytes' */
    if (db_state->db_nbytes <= 0) {   /* no more data in the buffer */
      if (nbytes <= BUFFER_SIZE) {
        if ((db_state->db_nbytes=(*proc_read_bytes)
             (db_state, db_state->db_buf, BUFFER_SIZE))
            < (int)nbytes) {
          *error_code = ERROR_EOF;
          return(NULL);
        }
      } else {
        if ((db_state->db_nbytes = (*proc_read_bytes)
             (db_state, db_state->db_buf,nbytes)) < (int)nbytes) {
          *error_code = ERROR_EOF;
          return(NULL);
        }
      }
      db_state->db_ptr = db_state->db_buf;
    } else if ((int)nbytes <= (db_state->db_size-(db_state->db_ptr-db_state->db_buf))) {
      /* read extra data into the overflow buffer */
      if ((*proc_read_bytes)
          (db_state, db_state->db_ptr+db_state->db_nbytes, nbytes-db_state->db_nbytes) !=
          ((int)nbytes-db_state->db_nbytes)) {
        *error_code = ERROR_EOF;
        return(NULL);
      } else {
        db_state->db_nbytes = nbytes;
      }
    } else {
      /* move existing data and read extra data to fill the buffer */
      memmove(db_state->db_buf, db_state->db_ptr, db_state->db_nbytes);
      db_state->db_ptr = db_state->db_buf;
          
      if (nbytes <= BUFFER_SIZE) {
        db_state->db_nbytes += (*proc_read_bytes)
          (db_state, db_state->db_buf+db_state->db_nbytes,
           BUFFER_SIZE-db_state->db_nbytes);
      } else {
        db_state->db_nbytes += (*proc_read_bytes)
          (db_state, db_state->db_buf+db_state->db_nbytes,
           nbytes-db_state->db_nbytes);
      }
      if (db_state->db_nbytes < (int)nbytes) {
        *error_code = ERROR_EOF;
        return(NULL);
      }
    }
  }
  db_state->db_ptr += nbytes;
  db_state->db_byte_count += nbytes;
  db_state->db_nbytes -= nbytes;
  db_state->db_nbits = (db_state->db_nbytes <= 0) ? 0 : 8;
  return(db_state->db_ptr - (long)nbytes);
}

/*
 * Tables that can be used to eliminate some Arithmetic operations
 */
#ifdef USE_SMALL_TABLES
static unsigned char pow2minus1[] = {
  0, 1, 3, 7, 15, 31, 63, 127, 255};
#endif

#ifdef USE_ALL_TABLES
static unsigned char vb_tables[] = {
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,128,0,128,0,128,0,128,0,128,0,128,0,128,0,128,0,128,0,128,0,128,0,128,0,128,
  0,128,0,128,0,128,0,128,0,128,0,128,0,128,0,128,0,128,0,128,0,128,0,128,0,128,
  0,128,0,128,0,128,0,128,0,128,0,128,0,128,0,128,0,128,0,128,0,128,0,128,0,128,
  0,128,0,128,0,128,0,128,0,128,0,128,0,128,0,128,0,128,0,128,0,128,0,128,0,128,
  0,128,0,128,0,128,0,128,0,128,0,128,0,128,0,128,0,128,0,128,0,128,0,128,0,128,
  0,128,0,128,0,128,0,128,0,128,0,128,0,128,0,128,0,128,0,128,0,128,0,128,0,128,
  0,128,0,128,0,128,0,128,0,128,0,128,0,128,0,128,0,128,0,128,0,128,0,128,0,128,
  0,128,0,128,0,128,0,128,0,128,0,128,0,128,0,128,0,128,0,128,0,128,0,128,0,128,
  0,128,0,128,0,128,0,128,0,128,0,128,0,128,0,128,0,128,0,128,0,128,0,128,0,128,
  0,128,0,128,0,128,0,128,0,128,0,128,0,128,0,128,0,128,0,128,0,128,
  0,64,128,192,0,64,128,192,0,64,128,192,0,64,128,192,0,64,128,192,0,64,128,192,
  0,64,128,192,0,64,128,192,0,64,128,192,0,64,128,192,0,64,128,192,0,64,128,192,
  0,64,128,192,0,64,128,192,0,64,128,192,0,64,128,192,0,64,128,192,0,64,128,192,
  0,64,128,192,0,64,128,192,0,64,128,192,0,64,128,192,0,64,128,192,0,64,128,192,
  0,64,128,192,0,64,128,192,0,64,128,192,0,64,128,192,0,64,128,192,0,64,128,192,
  0,64,128,192,0,64,128,192,0,64,128,192,0,64,128,192,0,64,128,192,0,64,128,192,
  0,64,128,192,0,64,128,192,0,64,128,192,0,64,128,192,0,64,128,192,0,64,128,192,
  0,64,128,192,0,64,128,192,0,64,128,192,0,64,128,192,0,64,128,192,0,64,128,192,
  0,64,128,192,0,64,128,192,0,64,128,192,0,64,128,192,0,64,128,192,0,64,128,192,
  0,64,128,192,0,64,128,192,0,64,128,192,0,64,128,192,0,64,128,192,0,64,128,192,
  0,64,128,192,0,64,128,192,0,64,128,192,0,64,128,192,
  0,32,64,96,128,160,192,224,0,32,64,96,128,160,192,224,0,32,64,96,128,160,192,
  224,0,32,64,96,128,160,192,224,0,32,64,96,128,160,192,224,0,32,64,96,128,160,
  192,224,0,32,64,96,128,160,192,224,0,32,64,96,128,160,192,224,0,32,64,96,128,
  160,192,224,0,32,64,96,128,160,192,224,0,32,64,96,128,160,192,224,0,32,64,96,
  128,160,192,224,0,32,64,96,128,160,192,224,0,32,64,96,128,160,192,224,0,32,64,
  96,128,160,192,224,0,32,64,96,128,160,192,224,0,32,64,96,128,160,192,224,0,32,
  64,96,128,160,192,224,0,32,64,96,128,160,192,224,0,32,64,96,128,160,192,224,
  0,32,64,96,128,160,192,224,0,32,64,96,128,160,192,224,0,32,64,96,128,160,192,
  224,0,32,64,96,128,160,192,224,0,32,64,96,128,160,192,224,0,32,64,96,128,160,
  192,224,0,32,64,96,128,160,192,224,0,32,64,96,128,160,192,224,0,32,64,96,128,
  160,192,224,0,32,64,96,128,160,192,224,0,32,64,96,128,160,192,224,0,32,64,96,
  128,160,192,224,
  0,16,32,48,64,80,96,112,128,144,160,176,192,208,224,240,0,16,32,48,64,80,96,
  112,128,144,160,176,192,208,224,240,0,16,32,48,64,80,96,112,128,144,160,176,
  192,208,224,240,0,16,32,48,64,80,96,112,128,144,160,176,192,208,224,240,0,16,
  32,48,64,80,96,112,128,144,160,176,192,208,224,240,0,16,32,48,64,80,96,112,
  128,144,160,176,192,208,224,240,0,16,32,48,64,80,96,112,128,144,160,176,192,
  208,224,240,0,16,32,48,64,80,96,112,128,144,160,176,192,208,224,240,0,16,32,
  48,64,80,96,112,128,144,160,176,192,208,224,240,0,16,32,48,64,80,96,112,128,
  144,160,176,192,208,224,240,0,16,32,48,64,80,96,112,128,144,160,176,192,208,
  224,240,0,16,32,48,64,80,96,112,128,144,160,176,192,208,224,240,0,16,32,48,64,
  80,96,112,128,144,160,176,192,208,224,240,0,16,32,48,64,80,96,112,128,144,160,
  176,192,208,224,240,0,16,32,48,64,80,96,112,128,144,160,176,192,208,224,240,0,
  16,32,48,64,80,96,112,128,144,160,176,192,208,224,240,
  0,8,16,24,32,40,48,56,64,72,80,88,96,104,112,120,128,136,144,152,160,168,176,
  184,192,200,208,216,224,232,240,248,0,8,16,24,32,40,48,56,64,72,80,88,96,104,
  112,120,128,136,144,152,160,168,176,184,192,200,208,216,224,232,240,248,0,8,
  16,24,32,40,48,56,64,72,80,88,96,104,112,120,128,136,144,152,160,168,176,184,
  192,200,208,216,224,232,240,248,0,8,16,24,32,40,48,56,64,72,80,88,96,104,112,
  120,128,136,144,152,160,168,176,184,192,200,208,216,224,232,240,248,0,8,16,24,
  32,40,48,56,64,72,80,88,96,104,112,120,128,136,144,152,160,168,176,184,192,
  200,208,216,224,232,240,248,0,8,16,24,32,40,48,56,64,72,80,88,96,104,112,120,
  128,136,144,152,160,168,176,184,192,200,208,216,224,232,240,248,0,8,16,24,32,
  40,48,56,64,72,80,88,96,104,112,120,128,136,144,152,160,168,176,184,192,200,
  208,216,224,232,240,248,0,8,16,24,32,40,48,56,64,72,80,88,96,104,112,120,128,
  136,144,152,160,168,176,184,192,200,208,216,224,232,240,248,
  0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60,64,68,72,76,80,84,88,92,96,100,
  104,108,112,116,120,124,128,132,136,140,144,148,152,156,160,164,168,172,176,
  180,184,188,192,196,200,204,208,212,216,220,224,228,232,236,240,244,248,252,
  0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60,64,68,72,76,80,84,88,92,96,100,
  104,108,112,116,120,124,128,132,136,140,144,148,152,156,160,164,168,172,176,
  180,184,188,192,196,200,204,208,212,216,220,224,228,232,236,240,244,248,252,
  0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60,64,68,72,76,80,84,88,92,96,100,
  104,108,112,116,120,124,128,132,136,140,144,148,152,156,160,164,168,172,176,
  180,184,188,192,196,200,204,208,212,216,220,224,228,232,236,240,244,248,252,
  0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60,64,68,72,76,80,84,88,92,96,100,
  104,108,112,116,120,124,128,132,136,140,144,148,152,156,160,164,168,172,176,
  180,184,188,192,196,200,204,208,212,216,220,224,228,232,236,240,244,248,252,
  0,2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,32,34,36,38,40,42,44,46,48,50,52,
  54,56,58,60,62,64,66,68,70,72,74,76,78,80,82,84,86,88,90,92,94,96,98,100,102,
  104,106,108,110,112,114,116,118,120,122,124,126,128,130,132,134,136,138,140,
  142,144,146,148,150,152,154,156,158,160,162,164,166,168,170,172,174,176,178,
  180,182,184,186,188,190,192,194,196,198,200,202,204,206,208,210,212,214,216,
  218,220,222,224,226,228,230,232,234,236,238,240,242,244,246,248,250,252,254,
  0,2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,32,34,36,38,40,42,44,46,48,50,52,
  54,56,58,60,62,64,66,68,70,72,74,76,78,80,82,84,86,88,90,92,94,96,98,100,102,
  104,106,108,110,112,114,116,118,120,122,124,126,128,130,132,134,136,138,140,
  142,144,146,148,150,152,154,156,158,160,162,164,166,168,170,172,174,176,178,
  180,182,184,186,188,190,192,194,196,198,200,202,204,206,208,210,212,214,216,
  218,220,222,224,226,228,230,232,234,236,238,240,242,244,246,248,250,252,254
};
#endif

unsigned char DB_Get_Byte(DB_STATE *db_state)
{
  unsigned char r;

#ifdef DEBUG
  if (db_state->db_nbits > 8) {
    debug("Error in DB_Get_Bits, db_state->db_nbits > 8");
  }
#endif

  if (db_state->db_nbits == 8) {
#ifdef DEBUG
    if (db_state->db_nbytes <= 0) {
      debug("Error in DB_Get_Byte, negative db_state->db_nbytes");
    }
#endif
    r = *db_state->db_ptr++; 
    db_state->db_byte_count++;
    UPDATE_BUFFER;
    if ((r == 0xff) && (db_state->db_nbytes > 0) && (*db_state->db_ptr == 0x00)) {
      /* Check for 0xff00, if true -> remove next byte */
      if (db_state->db_push_back_ff00) {
        db_state->db_push_back_ff00 = 0;
      } else {
        *db_state->db_ptr++ = 0xff; 
        db_state->db_byte_count++;
        UPDATE_BUFFER;
      }
    }
    return(r);
  } else {
#if defined(USE_ALL_TABLES)
    r = vb_tables[(db_state->db_nbits << 8)+(*db_state->db_ptr)];
#else
#ifdef USE_SMALL_TABLES
    r = (unsigned char) 
      (((*db_state->db_ptr) & pow2minus1[db_state->db_nbits]) << (8-db_state->db_nbits));
#else
    r = (unsigned char)
      (((*db_state->db_ptr) & ((1 << db_state->db_nbits) - 1)) << (8-db_state->db_nbits));
#endif
#endif
    GO_TO_NEXT_ALIGNED_BYTE;
    return((unsigned char)(r + (*db_state->db_ptr >> db_state->db_nbits)));
  }
}


unsigned int DB_Get_Bits( DB_STATE *db_state,
int nbits)
{
  int r;
    
#ifdef DEBUG
  if ((nbits < 0) || (nbits > 16)) {
    debug("Error in DB_Get_Bits, negative or too large nbits");
  }
  if (db_state->db_nbits > 8) {
    debug("Error in DB_Get_Bits, db_state->db_nbits > 8");
  }
#endif

  if (nbits <= db_state->db_nbits) {
    db_state->db_nbits -= nbits;
#ifdef USE_SMALL_TABLES
    return((*db_state->db_ptr >> db_state->db_nbits) & pow2minus1[nbits]);
#else
    return((*db_state->db_ptr >> db_state->db_nbits) & ((1 << nbits) -1));
#endif
  } else if (nbits <= 8) {
    nbits -= db_state->db_nbits;
#ifdef USE_SMALL_TABLES
    r = ((*db_state->db_ptr) & pow2minus1[db_state->db_nbits]) << nbits;
#else
    r = ((*db_state->db_ptr) & ((1 << db_state->db_nbits) - 1)) << nbits;
#endif
    GO_TO_NEXT_ALIGNED_BYTE;
    db_state->db_nbits = 8 - nbits;
    return(r + (*db_state->db_ptr >> db_state->db_nbits));
  } else {  /* nbits > 8 */
    r = DB_Get_Bits(db_state,nbits-8);
    return((r << 8) + DB_Get_Byte(db_state));
  }
}


/*
 * Get rid of all bits in current byte.
 */
void DB_Align_Byte( DB_STATE *db_state )
{
#ifdef DEBUG
  if (db_state->db_nbits > 8) {
    debug("Error in Align_Byte, db_state->db_nbits > 8");
  }
#endif
        
  if (db_state->db_nbits < 8) {
    db_state->db_nbits = 8;
    db_state->db_ptr++; 
    db_state->db_byte_count++;
    UPDATE_BUFFER;
  }
}


/*
 * Skip all input bytes until the next byte is 0xff. db_state->db_ptr points to the
 * byte which follows the byte 0xff and this byte is neither 0x00 (stuffed
 * byte) nor 0xff (optional fill byte).
 * Return 0 if successful, -1 otherwise (EOF is reached).
 */
int DB_Skip_To_Next_Marker(DB_STATE *db_state)
{
 Start_DB_Skip:
  while ((db_state->db_nbytes-- > 0) && (*db_state->db_ptr++ != 0xff)) {
    db_state->db_byte_count++;
  }
  db_state->db_byte_count++;
  if (db_state->db_nbytes >= 0) {         /* successfully locate 0xff */
    if (db_state->db_nbytes == 0) {     /* out of data - try to read more */
      *db_state->db_buf = 0xff;
      if ((db_state->db_nbytes= (*proc_read_bytes)
           (db_state,(db_state->db_ptr = db_state->db_buf+1),db_state->db_size-1))
          == 0) {
        /* EOF is reached */
        return(-1);
      }
    }
    if ((*db_state->db_ptr == 0) || (*db_state->db_ptr == 0xff)) {
      /* Avoid stack overflow caused by a huge loop
         return(DB_Skip_To_Next_Marker(db_state));
      */
      goto Start_DB_Skip;
    } else {
      return(0);
    }
  } else {                /* failed - try to read more */
    *db_state->db_buf = *(db_state->db_ptr-1);
    if ((db_state->db_nbytes= (*proc_read_bytes)
         (db_state, (db_state->db_ptr = db_state->db_buf+1),db_state->db_size-1))==0) {
      /* EOF is reached */
      return(-1);
    } else {
      goto Start_DB_Skip;
    }
  }
}


/********************************** OUTPUT *******************************/
int u_to_g_table[] = {      /* u_to_g_table[i] = (i-128)*(-0.3441) */
  44, 44, 43, 43, 43, 42, 42, 42, 41, 41, 41, 40, 40, 40, 39, 39,
  39, 38, 38, 38, 37, 37, 36, 36, 36, 35, 35, 35, 34, 34, 34, 33,
  33, 33, 32, 32, 32, 31, 31, 31, 30, 30, 30, 29, 29, 29, 28, 28,
  28, 27, 27, 26, 26, 26, 25, 25, 25, 24, 24, 24, 23, 23, 23, 22,
  22, 22, 21, 21, 21, 20, 20, 20, 19, 19, 19, 18, 18, 18, 17, 17,
  17, 16, 16, 15, 15, 15, 14, 14, 14, 13, 13, 13, 12, 12, 12, 11,
  11, 11, 10, 10, 10,  9,  9,  9,  8,  8,  8,  7,  7,  7,  6,  6,
  6,  5,  5,  4,  4,  4,  3,  3,  3,  2,  2,  2,  1,  1,  1,  0,
  0,  0,  0,  0,  0, -1, -1, -1, -2, -2, -2, -3, -3, -3, -4, -4,
  -5, -5, -5, -6, -6, -6, -7, -7, -7, -8, -8, -8, -9, -9, -9,-10,
  -10,-10,-11,-11,-11,-12,-12,-12,-13,-13,-13,-14,-14,-14,-15,-15,
  -16,-16,-16,-17,-17,-17,-18,-18,-18,-19,-19,-19,-20,-20,-20,-21,
  -21,-21,-22,-22,-22,-23,-23,-23,-24,-24,-24,-25,-25,-25,-26,-26,
  -27,-27,-27,-28,-28,-28,-29,-29,-29,-30,-30,-30,-31,-31,-31,-32,
  -32,-32,-33,-33,-33,-34,-34,-34,-35,-35,-35,-36,-36,-37,-37,-37,
  -38,-38,-38,-39,-39,-39,-40,-40,-40,-41,-41,-41,-42,-42,-42,-43
};

int u_to_b_table[] = {      /* u_to_b_table[i] = (i-128)*(1.7718) */
  -226,-224,-222,-220,-219,-217,-215,-213,-212,-210,-208,-206,-205,-203,-201,-199,
  -197,-196,-194,-192,-190,-189,-187,-185,-183,-181,-180,-178,-176,-174,-173,-171,
  -169,-167,-166,-164,-162,-160,-158,-157,-155,-153,-151,-150,-148,-146,-144,-143,
  -141,-139,-137,-135,-134,-132,-130,-128,-127,-125,-123,-121,-119,-118,-116,-114,
  -112,-111,-109,-107,-105,-104,-102,-100,-98,-96,-95,-93,-91,-89,-88,-86,
  -84,-82,-81,-79,-77,-75,-73,-72,-70,-68,-66,-65,-63,-61,-59,-57,
  -56,-54,-52,-50,-49,-47,-45,-43,-42,-40,-38,-36,-34,-33,-31,-29,
  -27,-26,-24,-22,-20,-18,-17,-15,-13,-11,-10, -8, -6, -4, -3, -1,
  0,  2,  4,  5,  7,  9, 11, 12, 14, 16, 18, 19, 21, 23, 25, 27,
  28, 30, 32, 34, 35, 37, 39, 41, 43, 44, 46, 48, 50, 51, 53, 55,
  57, 58, 60, 62, 64, 66, 67, 69, 71, 73, 74, 76, 78, 80, 82, 83,
  85, 87, 89, 90, 92, 94, 96, 97, 99,101,103,105,106,108,110,112,
  113,115,117,119,120,122,124,126,128,129,131,133,135,136,138,140,
  142,144,145,147,149,151,152,154,156,158,159,161,163,165,167,168,
  170,172,174,175,177,179,181,182,184,186,188,190,191,193,195,197,
  198,200,202,204,206,207,209,211,213,214,216,218,220,221,223,225
};

int v_to_r_table[] = {      /* v_to_r_table[i] = (i-128)*(1.4021) */
  -178,-177,-176,-174,-173,-171,-170,-169,-167,-166,-164,-163,-162,-160,-159,-157,
  -156,-155,-153,-152,-150,-149,-148,-146,-145,-143,-142,-141,-139,-138,-136,-135,
  -134,-132,-131,-129,-128,-127,-125,-124,-122,-121,-120,-118,-117,-115,-114,-113,
  -111,-110,-108,-107,-106,-104,-103,-101,-100,-99,-97,-96,-94,-93,-92,-90,
  -89,-87,-86,-85,-83,-82,-80,-79,-78,-76,-75,-73,-72,-71,-69,-68,
  -66,-65,-63,-62,-61,-59,-58,-56,-55,-54,-52,-51,-49,-48,-47,-45,
  -44,-42,-41,-40,-38,-37,-35,-34,-33,-31,-30,-28,-27,-26,-24,-23,
  -21,-20,-19,-17,-16,-14,-13,-12,-10, -9, -7, -6, -5, -3, -2,  0,
  0,  1,  3,  4,  6,  7,  8, 10, 11, 13, 14, 15, 17, 18, 20, 21,
  22, 24, 25, 27, 28, 29, 31, 32, 34, 35, 36, 38, 39, 41, 42, 43,
  45, 46, 48, 49, 50, 52, 53, 55, 56, 57, 59, 60, 62, 63, 64, 66,
  67, 69, 70, 72, 73, 74, 76, 77, 79, 80, 81, 83, 84, 86, 87, 88,
  90, 91, 93, 94, 95, 97, 98,100,101,102,104,105,107,108,109,111,
  112,114,115,116,118,119,121,122,123,125,126,128,129,130,132,133,
  135,136,137,139,140,142,143,144,146,147,149,150,151,153,154,156,
  157,158,160,161,163,164,165,167,168,170,171,172,174,175,177,178
};

int v_to_g_table[] = {      /* v_to_g_table[i] = (i-128)*(-0.7142) */
  91, 91, 90, 89, 89, 88, 87, 86, 86, 85, 84, 84, 83, 82, 81, 81,
  80, 79, 79, 78, 77, 76, 76, 75, 74, 74, 73, 72, 71, 71, 70, 69,
  69, 68, 67, 66, 66, 65, 64, 64, 63, 62, 61, 61, 60, 59, 59, 58,
  57, 56, 56, 55, 54, 54, 53, 52, 51, 51, 50, 49, 49, 48, 47, 46,
  46, 45, 44, 44, 43, 42, 41, 41, 40, 39, 39, 38, 37, 36, 36, 35,
  34, 34, 33, 32, 31, 31, 30, 29, 29, 28, 27, 26, 26, 25, 24, 24,
  23, 22, 21, 21, 20, 19, 19, 18, 17, 16, 16, 15, 14, 14, 13, 12,
  11, 11, 10,  9,  9,  8,  7,  6,  6,  5,  4,  4,  3,  2,  1,  1,
  0,  0,  0, -1, -2, -3, -3, -4, -5, -5, -6, -7, -8, -8, -9,-10,
  -10,-11,-12,-13,-13,-14,-15,-15,-16,-17,-18,-18,-19,-20,-20,-21,
  -22,-23,-23,-24,-25,-25,-26,-27,-28,-28,-29,-30,-30,-31,-32,-33,
  -33,-34,-35,-35,-36,-37,-38,-38,-39,-40,-40,-41,-42,-43,-43,-44,
  -45,-45,-46,-47,-48,-48,-49,-50,-50,-51,-52,-53,-53,-54,-55,-55,
  -56,-57,-58,-58,-59,-60,-60,-61,-62,-63,-63,-64,-65,-65,-66,-67,
  -68,-68,-69,-70,-70,-71,-72,-73,-73,-74,-75,-75,-76,-77,-78,-78,
  -79,-80,-80,-81,-82,-83,-83,-84,-85,-85,-86,-87,-88,-88,-89,-90
};

void Prepare_New_Buffers_From_Supplied_Buffer(
DB_STATE *db_state)
{
  int i, size;

  size = db_state->db_image_w*db_state->db_ncomps;
  if (db_state->db_nrows_left >= db_state->db_nrow_bufs) {
    for (i = 0; i < db_state->db_nrow_bufs; i++) {
      db_state->db_row_bufs[i] = db_state->db_supplied_ptr;
      db_state->db_supplied_ptr += size;
    }
  } else {                /* supplied buffer does not have enough space */
    if (db_state->db_ncomps > 1) {
      for (i = 0; i < db_state->db_nrows_left; i++) {
        db_state->db_row_bufs[i] = db_state->db_supplied_ptr;
        db_state->db_supplied_ptr += size;
      }
      for (; i < db_state->db_nrow_bufs; i++) {
        db_state->db_row_bufs[i] = db_state->db_xtra_buf;
      }
    } else {
      unsigned char *p;
      p = db_state->db_xtra_buf;
      for (i = 0; i < db_state->db_nrow_bufs; i++) {
        db_state->db_row_bufs[i] = p;
        p += size;
      }
    }
  }
}

void Write_Current_MCU_Mono(void *vdb_state)
{
  int i, nrows;
  DB_STATE *db_state = (DB_STATE *)vdb_state;

  db_state->db_current_col += 8*db_state->db_max_hsampling;
  if (db_state->db_current_col >= db_state->db_image_w) {         /* buffers are full */
    if (db_state->db_nrows_left >= db_state->db_nrow_bufs) {
      nrows = db_state->db_nrow_bufs;
    } else if (db_state->db_nrows_left > 0) {
      /* Copy data from db_state->db_xtra_buf to db_supplied_buf */
      if ((db_state->db_ncomps != 3) && (db_state->db_supplied_bufs != NULL)) {
        for (i = 0; i < db_state->db_nrows_left; i++) {
          memcpy(db_state->db_supplied_ptr, db_state->db_row_bufs[i],
                 db_state->db_image_w*db_state->db_ncomps);
          db_state->db_supplied_ptr += db_state->db_image_w*db_state->db_ncomps;
        }
      }
      nrows = db_state->db_nrows_left;
    } else {
      nrows = 0;
    }
    db_state->db_nrows_left -= nrows;
    for (i = 0; i < nrows; i++) {
      if (proc_write_rows != NULL)
        (*proc_write_rows)(db_state->db_outptr, db_state->db_row_bufs[i],1,db_state->db_image_w*db_state->db_ncomps);
    }
    db_state->db_current_col = 0;

    if (db_state->db_supplied_bufs != NULL) {          /* prepare new buffers */
      Prepare_New_Buffers_From_Supplied_Buffer(db_state);
    }
  }
}


/*
 * Interpolate and convert YUV->RGB. General case.
 */
void Write_Current_MCU_General( DB_STATE *db_state )
{
  int i, j, scale_x0,scale_y0, scale_x1,scale_y1, scale_x2,scale_y2;
  int *src0, *src1, *src2;
  unsigned char *dest;
  int tmp = 0,
    y = 0,
    v_r = 0,
    u_g = 0,
    v_g = 0,
    u_b = 0;
  int x0, y0, x1, y1, x2, y2, ncols;

  scale_x0 = db_state->db_comps[0].scale_x;
  scale_y0 = db_state->db_comps[0].scale_y;
  scale_x1 = db_state->db_comps[1].scale_x;
  scale_y1 = db_state->db_comps[1].scale_y;
  scale_x2 = db_state->db_comps[2].scale_x;
  scale_y2 = db_state->db_comps[2].scale_y;
  src0     = db_state->db_comps[0].mcu_buf;
  src1     = db_state->db_comps[1].mcu_buf;
  src2     = db_state->db_comps[2].mcu_buf;

  if ((ncols = db_state->db_max_hsampling*8 - 
       (db_state->db_image_w - db_state->db_current_col)) < 0) {
    ncols = 0;
  }

  y0 = scale_y0;
  y1 = scale_y1;
  y2 = scale_y2;
  for (i = 0; i < db_state->db_nrow_bufs; i++) {
    dest = db_state->db_row_bufs[i] + db_state->db_current_col*3;
    if (++y0 < scale_y0) {
      src0 -= db_state->db_comps[0].hsampling*8;
    } else {
      y0 = 0;
    }
    if (++y1 < scale_y1) {
      src1 -= db_state->db_comps[1].hsampling*8;
    } else {
      y1 = 0;
    }
    if (++y2 < scale_y2) {
      src2 -= db_state->db_comps[2].hsampling*8;
    } else {
      y2 = 0;
    }

    x0 = scale_x0;
    x1 = scale_x1;
    x2 = scale_x2;
    for (j = db_state->db_max_hsampling*8; j > ncols; j--) {
      if (++x0 >= scale_x0) {
        x0 = 0;
        y = *src0++;
      }
      if (++x1 >= scale_x1) {
        x1  = 0;
        u_g = u_to_g_table[tmp = *src1++];
        u_b = u_to_b_table[tmp];
      }
      if (++x2 >= scale_x2) {
        x2  = 0;
        v_r = v_to_r_table[tmp = *src2++];
        v_g = v_to_g_table[tmp];
      }
      ASSIGN_AND_BOUND(tmp, y + v_r, *dest++);
      ASSIGN_AND_BOUND(tmp, y + u_g + v_g, *dest++);
      ASSIGN_AND_BOUND(tmp, y + u_b, *dest++);
    }
    for (; j > 0; j--) {
      if (++x0 >= scale_x0) {
        x0 = 0;
        src0++;
      }
      if (++x1 >= scale_x1) {
        x1 = 0;
        src1++;
      }
      if (++x2 >= scale_x2) {
        x2 = 0;
        src2++;
      }
    }
  }
  Write_Current_MCU_Mono(db_state);
}


/*
 * Return 0 if successful, -1 if memory can not be allocated.
 * -2 if other errors.
 */
int DB_Write_Begin(
                   DB_STATE *db_state,
                   int ncomps,         /* number of image components (must be 1,2,3 or 4) */
                   int horMCU,         /* total number of MCU in hor. direction */
                   int width,          /* image width */
                   int height,         /* image height */
                   int *hsampling,     /* # hor. data units in 1 MCU of each component */
                   int *vsampling,     /* # ver. data units in 1 MCU of each component */
                   DECODER_STRUCT *decoder
)
{
  int i;

  if ((ncomps != 1) && (ncomps != 2) && 
      (ncomps != 3) && (ncomps != 4) ) return(-2);
  db_state->db_supplied_bufs = NULL;
  db_state->db_xtra_buf = NULL;
  db_state->db_ncomps = ncomps;
  db_state->db_image_w = width;
  db_state->db_nrows_left = height;
  db_state->db_current_col = 0;
  db_state->db_max_hsampling = hsampling[0];
  db_state->db_max_vsampling = vsampling[0];
  if (db_state->db_ncomps > 1) {
    for (i = 0; i < ncomps; i++) {
      if (db_state->db_max_hsampling <hsampling[i])
        db_state->db_max_hsampling=hsampling[i];
      if (db_state->db_max_vsampling <vsampling[i])
        db_state->db_max_vsampling=vsampling[i];
      db_state->db_comps[i].hsampling = hsampling[i];
      db_state->db_comps[i].vsampling = vsampling[i];
      db_state->db_comps[i].block_x = -1;
      db_state->db_comps[i].block_y =  0;
      db_state->db_comps[i].offset_to_next_row = 8*(hsampling[i] - 1);
      if (decoder->mcu_buf[i] == NULL) {
        if ((db_state->db_comps[i].mcu_buf = (int *) FPX_malloc(hsampling[i]*
                                                                vsampling[i]*64*sizeof(int))) == NULL) {
          for (i--; i >= 0; i--) {
            if(db_state->db_comps[i].mcu_buf) { /* CHG_JPEG_MEM_FIX - added check & clear */
              FPX_free(db_state->db_comps[i].mcu_buf);
              db_state->db_comps[i].mcu_buf = NULL;
            }
          }
          return(-1);
        }
        decoder->mcu_buf[i] = db_state->db_comps[i].mcu_buf;
      } else {
        db_state->db_comps[i].mcu_buf = decoder->mcu_buf[i];
      }
    }
    db_state->db_nrow_bufs = 8*db_state->db_max_vsampling;
    decoder->nrow_bufs = db_state->db_nrow_bufs;
    for (i = 0; i < ncomps; i++) {
      db_state->db_comps[i].scale_x = 
        (db_state->db_max_hsampling+hsampling[i]-1) / hsampling[i];
      db_state->db_comps[i].scale_y = 
        (db_state->db_max_vsampling+vsampling[i]-1) / vsampling[i];
    }

    /* Allocate db_state->db_nrow_bufs output rows */
    if (decoder->row_bufs == NULL) {
      if ((db_state->db_row_bufs = (unsigned char **) FPX_calloc(db_state->db_nrow_bufs,
                                                                 sizeof(unsigned char *))) == NULL) {
        for (i = 0; i < ncomps; i++)
          if(db_state->db_comps[i].mcu_buf) { /* CHG_JPEG_MEM_FIX - added check & clear */
            FPX_free(db_state->db_comps[i].mcu_buf);
            db_state->db_comps[i].mcu_buf = NULL;
          }
        return(-1);
      }
      decoder->row_bufs = db_state->db_row_bufs;
    } else {
      db_state->db_row_bufs = decoder->row_bufs;
    }

    for (i = 0; i < db_state->db_nrow_bufs; i++) db_state->db_row_bufs[i] = NULL;
  } else { /* Grayscale Image */
    db_state->db_comps[0].hsampling = hsampling[0];
    db_state->db_comps[0].vsampling = vsampling[0];
    db_state->db_comps[0].block_x = -1;
    db_state->db_comps[0].block_y =  0;
    db_state->db_nrow_bufs = 8*db_state->db_max_vsampling;
    db_state->db_comps[0].scale_x = 
      (db_state->db_max_hsampling+hsampling[0]-1)/hsampling[0];
    db_state->db_comps[0].scale_y = 
      (db_state->db_max_vsampling+vsampling[0]-1)/vsampling[0];

    if ((db_state->db_row_bufs = (unsigned char **) FPX_calloc(db_state->db_nrow_bufs,
                                                               sizeof(unsigned char *))) == NULL) {
      return(-1);
    }
    for (i = 0; i < db_state->db_nrow_bufs; i++) db_state->db_row_bufs[i] = NULL;
    db_state->db_comps[0].offset_to_next_row = db_state->db_image_w;
  }

  return(0);
}


int DB_Allocate_Output_Buffers(
                               DB_STATE *db_state,
                               DECODER_STRUCT *decoder)
{
  int i;
  unsigned char *rows;

  if (db_state->db_supplied_bufs != NULL) { /* use it as output buffers */
    /* Allocate xtra buffer */
    if (db_state->db_ncomps > 1) {
      db_state->db_xtra_buf = (unsigned char *)FPX_malloc(
                                                          (size_t)db_state->db_image_w*db_state->db_ncomps*sizeof(unsigned char));
    } else {
      db_state->db_xtra_buf = (unsigned char *) FPX_malloc(db_state->db_nrow_bufs*
                                                           (size_t)db_state->db_image_w*sizeof(unsigned char));
    }
    if (db_state->db_xtra_buf == NULL) return(ERROR_MEM);
    Prepare_New_Buffers_From_Supplied_Buffer(db_state);
  } else {                   /* need to allocate output buffers */
    if (db_state->db_ncomps > 1) {
      for (i = 0; i < db_state->db_nrow_bufs; i++) {
        if (decoder->irow_bufs[i] == NULL) {
          if ((db_state->db_row_bufs[i] = (unsigned char *) FPX_malloc(
                                                                       (size_t)db_state->db_image_w*db_state->db_ncomps*sizeof(unsigned char)))
              == NULL){
            for (i--; i >= 0; i--) {
              FPX_free(db_state->db_row_bufs[i]); 
              db_state->db_row_bufs[i] = NULL;
            }
          }
          decoder->irow_bufs[i] = db_state->db_row_bufs[i];
        } else {
          db_state->db_row_bufs[i] = decoder->irow_bufs[i];
        }
      }
    } else {
      if (decoder->irow_bufs[0] == NULL) {
        if ((rows = (unsigned char *) FPX_malloc(db_state->db_nrow_bufs*
                                                 (size_t)db_state->db_image_w*sizeof(unsigned char))) == NULL) {
          return(ERROR_MEM);
        }
        decoder->irow_bufs[0] = rows;
      } else {
        rows = decoder->irow_bufs[0];
      }
      for (i = 0; i < db_state->db_nrow_bufs; i++) {
        db_state->db_row_bufs[i] = rows + i*db_state->db_image_w;
      }
    }
  }
  return(0);
}


void DB_Write_End( DB_STATE *db_state )
{
  int i;
    
  if (db_state->db_ncomps > 1) {
    for (i = 0; i < db_state->db_ncomps; i++) {
      if (db_state->db_comps[i].mcu_buf != NULL) {
        FPX_free(db_state->db_comps[i].mcu_buf);
        db_state->db_comps[i].mcu_buf = NULL;
      }
    }
    if (db_state->db_row_bufs != NULL) {
      if (db_state->db_supplied_bufs == NULL) {
        for (i = 0; i < db_state->db_nrow_bufs; i++) {
          if (db_state->db_row_bufs[i] != NULL) {
            FPX_free(db_state->db_row_bufs[i]);
            db_state->db_row_bufs[i] = NULL;
          }
        };
      } else {
        if (db_state->db_xtra_buf != NULL) {
          FPX_free(db_state->db_xtra_buf);
          db_state->db_xtra_buf = NULL;
        }
        db_state->db_xtra_buf = NULL;
      }
      FPX_free(db_state->db_row_bufs);
      db_state->db_row_bufs = NULL;
    }
  } else {
    if (db_state->db_row_bufs != NULL) {
      if (db_state->db_supplied_bufs == NULL) {
        if (db_state->db_row_bufs[0] != NULL) FPX_free(db_state->db_row_bufs[0]);
      } else {
        if (db_state->db_xtra_buf != NULL) FPX_free(db_state->db_xtra_buf);
        db_state->db_xtra_buf = NULL;
      }
      FPX_free(db_state->db_row_bufs);
      db_state->db_row_bufs = NULL;
    }
  }
  db_state->db_ncomps = 0;
}

void DB_Setup_Data_Unit( DB_STATE *db_state,
                         int comp )
{
  BCOMPONENT *bcomp;

  bcomp = db_state->db_comps + comp;
  /* Update block_x and block_y */
  if (++(bcomp->block_x) >= bcomp->hsampling) {
    bcomp->block_x = 0;
    if (++(bcomp->block_y) >= bcomp->vsampling) bcomp->block_y = 0;
  }
  if (db_state->db_ncomps == 3) {
    db_state->db_current_buf = bcomp->mcu_buf + 64L*bcomp->block_y*bcomp->hsampling
      + 8*bcomp->block_x;
    db_state->db_do_bound = comp;
  } else {
    db_state->db_current_buf2 = db_state->db_row_bufs[8*bcomp->block_y] +
      (db_state->db_do_bound = db_state->db_current_col + 8*bcomp->block_x);
    db_state->db_do_bound = db_state->db_image_w - db_state->db_do_bound;
  }
  db_state->db_offset_to_next_row = bcomp->offset_to_next_row;
}


void DB_Write_Current_MCU( DB_STATE *db_state,
int interleave )
{
  (*db_state->proc_write_current_mcu)((void *)db_state,interleave);
}

/*
 * Return 0 if successful, -1 o.w.
 */
int DB_Write_Scan_MCUs(
                       DB_STATE *db_state,
                       int width,   /* image width */
                       int height,  /* image height */
                       int interleave  /* Interleave flag */
)
{
  /* void Write_Scan_MCUs_Mono();*/

  if ((db_state->db_ncomps != 1) && (db_state->db_ncomps != 2) && 
      (db_state->db_ncomps != 3) && (db_state->db_ncomps != 4) ) 
    return(ERROR_IMG_FORMAT);

  if (db_state->db_ncomps == 4) {
    if ( 
        (db_state->db_comps[0].scale_x == 1) && (db_state->db_comps[0].scale_y == 1) && 
        (db_state->db_comps[1].scale_x == 1) && (db_state->db_comps[1].scale_y == 1) &&
        (db_state->db_comps[2].scale_x == 1) && (db_state->db_comps[2].scale_y == 1) && 
        (db_state->db_comps[3].scale_x == 1) && (db_state->db_comps[3].scale_y == 1) ) {
      Write_Scan_MCUs_1111(db_state->db_outbuf,(int *)db_state->db_MCUbuf,
                           width,height,interleave);
    } else if (
               (db_state->db_comps[0].scale_x == 1) && (db_state->db_comps[0].scale_y == 1) && 
               (db_state->db_comps[1].scale_x == 2) && (db_state->db_comps[1].scale_y == 2) &&
               (db_state->db_comps[2].scale_x == 2) && (db_state->db_comps[2].scale_y == 2) && 
               (db_state->db_comps[3].scale_x == 1) && (db_state->db_comps[3].scale_y == 1) ) {
      Write_Scan_MCUs_4114(db_state->db_outbuf,(int *)db_state->db_MCUbuf,
                           width,height,interleave);
    } else if (
               (db_state->db_comps[0].scale_x == 1) && (db_state->db_comps[0].scale_y == 1) && 
               (db_state->db_comps[1].scale_x == 2) && (db_state->db_comps[1].scale_y == 1) &&
               (db_state->db_comps[2].scale_x == 2) && (db_state->db_comps[2].scale_y == 1) && 
               (db_state->db_comps[3].scale_x == 1) && (db_state->db_comps[3].scale_y == 1) ) {
      Write_Scan_MCUs_4224(db_state->db_outbuf,(int *)db_state->db_MCUbuf,
                           width,height,interleave);
    } else {
      return(ERROR_IMG_FORMAT);
    }
  } else if (db_state->db_ncomps == 3) {
    if ( 
        (db_state->db_comps[0].scale_x == 1) && (db_state->db_comps[0].scale_y == 1) && 
        (db_state->db_comps[1].scale_x == 2) && (db_state->db_comps[1].scale_y == 2) &&
        (db_state->db_comps[2].scale_x == 2) && (db_state->db_comps[2].scale_y == 2) ) {
      Write_Scan_MCUs_211(db_state->db_outbuf, (int *)db_state->db_MCUbuf,
                          width,height,interleave);
    } else if (  
               (db_state->db_comps[0].scale_x == 1) && (db_state->db_comps[0].scale_y == 1) && 
               (db_state->db_comps[1].scale_x == 2) && (db_state->db_comps[1].scale_y == 1) &&
               (db_state->db_comps[2].scale_x == 2) && (db_state->db_comps[2].scale_y == 1) ) {
      Write_Scan_MCUs_422(db_state->db_outbuf, (int *)db_state->db_MCUbuf,
                          width,height,interleave);
    } else if (  
               (db_state->db_comps[0].scale_x == 1) && (db_state->db_comps[0].scale_y == 1) && 
               (db_state->db_comps[1].scale_x == 1) && (db_state->db_comps[1].scale_y == 1) &&
               (db_state->db_comps[2].scale_x == 1) && (db_state->db_comps[2].scale_y == 1) ) {
      Write_Scan_MCUs_111(db_state->db_outbuf, (int *)db_state->db_MCUbuf,
                          width,height,interleave);
    } else {
      return(ERROR_IMG_FORMAT);
    }
  } else if (db_state->db_ncomps == 2) {
    if ( 
        (db_state->db_comps[0].scale_x == 1) && (db_state->db_comps[0].scale_y == 1) && 
        (db_state->db_comps[1].scale_x == 1) && (db_state->db_comps[1].scale_y == 1) ) {
      Write_Scan_MCUs_11(db_state->db_outbuf, (int *)db_state->db_MCUbuf,
                         width,height,interleave);
    } else {
      return(ERROR_IMG_FORMAT);
    }
  } else if (db_state->db_ncomps == 1) {
    Write_Scan_MCUs_Mono(db_state->db_outbuf, (int *)db_state->db_MCUbuf,width,height);
  }
  return(0);
}
