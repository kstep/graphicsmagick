/* -*-C-*-
******************************************************************************
*
* File:         dbuffer.h
* RCS:          $Header$
* Description:  see dbuffer.c
* Author:       Hugh P. Nguyen
* Created:      Fri Dec  4 11:04:22 1992
* Modified:     Thu Aug 10 11:04:53 1995 by Gregory S. Yovanof
* Language:     C
* Package:      Hewlett-Packard JPEG Encoder/Decoder
*
* Copyright (c) 1999 Digital Imaging Group, Inc.
* For conditions of distribution and use, see copyright notice
* in Flashpix.h
*
******************************************************************************
*/

typedef struct {            /* one structure for each image component */
  int *mcu_buf;               /* buffer of current MCU */
  int hsampling;              /* # horizontal data units in one MCU */
  int vsampling;              /* # vertical data units in one MCU */
  int scale_x;                /* scale factor = max(hsamplings)/hsampling */
  int scale_y;                /* scale factor = max(vsamplings)/vsampling */
  int block_x, block_y;       /* position of current 8x8 block within MCU */
  int offset_to_next_row;
} BCOMPONENT;

typedef struct {
  unsigned char *db_buf; /* local data buffer */
  unsigned char *db_ptr; /* points to next byte with unprocessed bits
                            in db_buf*/
  int db_size;           /* local buffer size = BUFFER_SIZE + extra */
  unsigned char *db_inbuf;   /* memory-buffer with compressed data */
  unsigned char *db_inptr;   /* pointer to db_inbuf */
  int db_inbuf_size;     /* size of compressed data buffer */
  long db_byte_count;    /* total number of processed bytes from inbuf */
  unsigned char *db_supplied_bufs; /* externally allocated out-buffer */
  unsigned char *db_supplied_ptr;  /* pointer to externally supplied buffer */
  unsigned char *db_outbuf;   /* output image buffer */
  unsigned char *db_outptr;   /* pointer to db_outbuf */
  unsigned int *db_MCUbuf;   /* output buffer sequential MCU order */
  unsigned int *db_MCUptr;   /* pointer to db_MCUbuf */
  int db_nbytes;         /* number of unprocessed bytes in db_buf, 
                          including the 1st byte that has some bits
                            already processed */
  int db_nbits;          /* number of unprocessed bits in *db_ptr,
                            in [0,8] inclusive */
  long nbytes_left;      /* number of unprocessed bytes in inbuf */
  int db_push_back_ff00; /* normally, bytes ff00 is replaced by byte ff.
                          In rare instances, this replacement causes
                          another replacement so that ff0000 becomes
                          just ff.  This happens only if after the
                          first replacement, byte ff is pushed back 
                          and the next byte is 00.  This boolean
                          variable is set true in this case, so that
                            the next replacement will not happen */
  /*
                            db_nbits
                               v
    |BYTE BYTE BYTE BYTE BYTE|BYTE BYTE BYTE BYTE BYTE BYTE BYTE BYTE|
    ^                        ^
  db_buf                  db_ptr
                             |<------------- db_nbytes ------------->|
*/

  /** User provided IO routines **/

  /**** Information about Output Image ****/
  int db_image_w;             /* image width */
  int db_ncomps;              /* number of components in the image */
  BCOMPONENT db_comps[4];     /* image components (1,2,3 or 4) */
  int db_max_hsampling;       /* max. hsampling of 'db_ncomps' components */
  int db_max_vsampling;       /* max. vsampling of 'db_ncomps' components */
  int db_current_col;         /* used to determine if db_our_rows are full */
  unsigned char **db_row_bufs;/* output buffers */
  unsigned char *db_xtra_buf; /* extra buffer used if db_supplied_bufs!=NULL */
  int db_nrow_bufs;           /* number of row buffers */
  int db_nrows_left;          /* number of rows to be written */
  void (*proc_write_current_mcu)(void *, int);
  int *db_current_buf;
  unsigned char *db_current_buf2;
  int db_offset_to_next_row;
  int db_do_bound; /* has diff. meanings depending on db_state->db_ncomps */
} DB_STATE;


JPEGEXPORT
int            DB_Begin ( DB_STATE *db_state );
JPEGEXPORT
unsigned char *DB_Get_Data( DB_STATE *db_state,
unsigned int nbytes, int *error_code);
JPEGEXPORT
unsigned char  DB_Get_Byte(DB_STATE *db_state );
JPEGEXPORT
unsigned int   DB_Get_Bits(DB_STATE *db_state, int nbits);
JPEGEXPORT
void           DB_Align_Byte( DB_STATE *db_state );
JPEGEXPORT
int            DB_Skip_To_Next_Marker( DB_STATE *db_state );
JPEGEXPORT
long           DB_Processed_Bytes( DB_STATE *db_state );
JPEGEXPORT
void           DB_End( DB_STATE *db_state );

JPEGEXPORT
void DB_Init(DB_STATE *db_state, 
int  (*read_bytes)(void *, void *, int),
void (*write_rows)(void *, void *, int, int),
unsigned char *data_in,
size_t data_in_size,
unsigned char *data_out,
int interleave,
int which  /* 1: set read_bytes */
/* 2: set write_rows & data */
);

JPEGEXPORT
int  DB_Write_Begin(
DB_STATE *db_state,
int ncomps,         /* number of image components (must be 1,2,3 or 4) */
int horMCU,         /* total number of MCU in hor. direction */
int width,          /* image width */
int height,         /* image height */
int *hsampling,     /* # hor. data units in 1 MCU of each component */
int *vsampling,     /* # ver. data units in 1 MCU of each component */
DECODER_STRUCT *decoder
);

JPEGEXPORT
void DB_Setup_Data_Unit( DB_STATE *db_state, int comp);

JPEGEXPORT
void DB_Write_Current_MCU( DB_STATE *db_state,  int interleave );

JPEGEXPORT
void DB_Write_End( DB_STATE *db_state );

JPEGEXPORT
void Write_Scan_MCUs_11(
unsigned char *outbuf, /* image out */
int *MCUbuf, /* sequence of MCUs */
int width,   /* image width */
int height,  /* image height */
int interleave  /* Interleave flag */
);

JPEGEXPORT
void Write_Scan_MCUs_1111(
unsigned char *outbuf, /* image out */
int *MCUbuf, /* sequence of MCUs */
int width,   /* image width */
int height,  /* image height */
int interleave  /* Interleave flag */
);

JPEGEXPORT
void Write_Scan_MCUs_4114(
unsigned char *outbuf, /* image out */
int *MCUbuf, /* sequence of MCUs */
int width,   /* image width */
int height,  /* image height */
int interleave  /* Interleave flag */
);

JPEGEXPORT
void Write_Scan_MCUs_111(
unsigned char *outbuf, /* image out */
int *MCUbuf, /* sequence of MCUs */
int width,   /* image width */
int height,  /* image height */
int interleave  /* Interleave flag */
);

JPEGEXPORT
void Write_Scan_MCUs_211(   
unsigned char *outbuf, /* image out */
int *MCUbuf, /* sequence of MCUs */
int width,   /* image width */
int height,  /* image height */
int interleave  /* Interleave flag */
);

JPEGEXPORT
void Write_Scan_MCUs_422(   
unsigned char *outbuf, /* image out */
int *MCUbuf, /* sequence of MCUs */
int width,   /* image width */
int height,  /* image height */
int interleave  /* Interleave flag */
);

JPEGEXPORT
void Write_Scan_MCUs_4224(   
unsigned char *outbuf, /* image out */
int *MCUbuf, /* sequence of MCUs */
int width,   /* image width */
int height,  /* image height */
int interleave  /* Interleave flag */
);

JPEGEXPORT
void Write_Scan_MCUs_Mono(
unsigned char *outbuf, /* image out */
int *MCUbuf, /* sequence of MCUs */
int width,   /* image width */
int height  /* image height */);

JPEGEXPORT
int DB_Write_Scan_MCUs(  
DB_STATE *db_state,
int width,          /* image width */
int height,         /* image height */
int interleave  /* Interleave flag */
);

JPEGEXPORT
int DB_Allocate_Output_Buffers(
DB_STATE *db_state,
DECODER_STRUCT *decoder );

/* Push back 'nbits' bits to input data buffer */
/*
 * Note: if current byte is 0x00 and last byte is 0xff, then the last byte
 *       must be 0xff00; otherwise, the current byte 0x00 is already removed.
 *       In this case, push_back_ff00 is set TRUE. It will be set FALSE when
 *       the current byte (next byte after PUSH_BACK_BITS) 0x00 is retrieved.
 */
#define PUSH_BACK_BITS(nbits)\
    if ((db_state->db_nbits = db_state->db_nbits + (nbits)) > 8) {\
        db_state->db_nbits -= 8;\
        if ((*db_state->db_ptr-- == 0x00) && (*db_state->db_ptr == 0xff)) {\
            db_state->db_push_back_ff00 = 1;\
        };\
        db_state->db_nbytes++;\
    }

#define ASSIGN_AND_BOUND(tmp, in, out)\
    if ((tmp = in) <= 0) {\
        out = 0;\
    } else if (tmp >= 255) {\
        out = 255;\
    } else {\
        out = tmp;\
    }

/* Shift data and check Bound then store it in 'out' */
#define SHIFT_AND_BOUND(tmp, in, out)\
    if ((tmp = in + 128) <= 0) {\
        out = 0;\
    } else if (tmp >= 255) {\
        out = 255;\
    } else {\
        out = tmp;\
    }

/* Shift data, check Bound then write them to current output row */
#define WRITE_ROW(buf,tmp,data0,data1,data2,data3,data4,data5,data6,data7)\
if (db_state->db_ncomps > 1) {\
    register int *p;\
    p = (int *)buf;\
    if (db_state->db_do_bound) {\
        SHIFT_AND_BOUND(tmp, data0, *p++);\
        SHIFT_AND_BOUND(tmp, data1, *p++);\
        SHIFT_AND_BOUND(tmp, data2, *p++);\
        SHIFT_AND_BOUND(tmp, data3, *p++);\
        SHIFT_AND_BOUND(tmp, data4, *p++);\
        SHIFT_AND_BOUND(tmp, data5, *p++);\
        SHIFT_AND_BOUND(tmp, data6, *p++);\
        SHIFT_AND_BOUND(tmp, data7, *p++);\
    } else {\
        *p++ = data0 + 128;\
        *p++ = data1 + 128;\
        *p++ = data2 + 128;\
        *p++ = data3 + 128;\
        *p++ = data4 + 128;\
        *p++ = data5 + 128;\
        *p++ = data6 + 128;\
        *p++ = data7 + 128;\
    }\
} else {\
    register int *p;\
    p = buf;\
    if (db_state->db_do_bound >= 8) {\
        SHIFT_AND_BOUND(tmp, data0, *p++);\
        SHIFT_AND_BOUND(tmp, data1, *p++);\
        SHIFT_AND_BOUND(tmp, data2, *p++);\
        SHIFT_AND_BOUND(tmp, data3, *p++);\
        SHIFT_AND_BOUND(tmp, data4, *p++);\
        SHIFT_AND_BOUND(tmp, data5, *p++);\
        SHIFT_AND_BOUND(tmp, data6, *p++);\
        SHIFT_AND_BOUND(tmp, data7, *p++);\
    } else {\
        int data_a[8], *q;\
        int ncols;\
        data_a[0] = data0;\
        data_a[1] = data1;\
        data_a[2] = data2;\
        data_a[3] = data3;\
        data_a[4] = data4;\
        data_a[5] = data5;\
        data_a[6] = data6;\
        data_a[7] = data7;\
        for (q = data_a, ncols = db_state->db_do_bound; ncols > 0; ncols--) {\
            SHIFT_AND_BOUND(tmp, *q++, *p++);\
        }\
    }\
}

/* Shift data, check Bound then write them to current output row */
#define WRITE_ROWS(buf,data)\
if (db_state->db_ncomps > 1) {\
    register int *p, i, val;\
    if (db_state->db_do_bound) {\
        SHIFT_AND_BOUND(val, data, val);\
    } else val = data + 128;\
    p = (int *)buf;\
    for (i = 8; i > 0; i--) {\
        *p++ = val;\
        *p++ = val;\
        *p++ = val;\
        *p++ = val;\
        *p++ = val;\
        *p++ = val;\
        *p++ = val;\
        *p++ = val;\
    }\
} else {\
    register unsigned char *p, val;\
    register int i, j;\
    SHIFT_AND_BOUND(i, data, val);\
    if (db_state->db_do_bound > 8) db_state->db_do_bound = 8;\
    for (i = 8; i > 0; i--) {\
        p = (int *)buf;\
        for (j = db_state->db_do_bound; j > 0; j--) {\
            *p++ = val;\
        }\
    }\
}
