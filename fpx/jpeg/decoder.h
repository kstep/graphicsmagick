/* -*-C-*-
******************************************************************************
*
* File:         decoder.h
* RCS:          $Header$
* Description:  main module of JPEG decoder library
* Author:       Peyma Oskoui
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

#ifndef _DECODER_H
#define _DECODER_H

#define FOUR_ONE_ONE      411
#define FOUR_ONE_ONE_FOUR   4114
#define FOUR_TWO_TWO      422
#define FOUR_TWO_TWO_FOUR   4224
#define WRONG_SAMPLE      0
#define ONE_ONE_ONE       111
#define ONE_ONE_ONE_ONE     1111
#define COLOR_CONVERT     10000
#define COLOR_AND_FOUR_ONE_ONE    10411
#define COLOR_AND_FOUR_ONE_ONE_FOUR 14114
#define COLOR_AND_FOUR_TWO_TWO    10422
#define COLOR_AND_FOUR_TWO_TWO_FOUR 14224
#define COLOR_AND_ONE_ONE_ONE   10111
#define COLOR_AND_ONE_ONE_ONE_ONE 11111

#define ENTROPY_ONLY    0
#define ENTROPY_AND_HEADER  1

#define CHEN_DCT    0
#define WINOGRAD    1
#define PRUNED_WINOGRAD   2

#ifndef _CLIP_
#define CLIP(i) ( (i)>255?255:(((i)<0)?0:(i)) )
#define _CLIP_
#endif

#define LEFT_NIBBLE(x)    ( (0x00F0 & x) >> 4)
#define RIGHT_NIBBLE(x)   ( (0x000F & x) )

/* CHG_FILE_ERR - unified JPEG error values by offsetting the values by 0x0400 */
/* error value */
#define DJPEG_NO_ERROR        0x0000
#define DJPEG_ERR_ALLOC_DECODER   0x0402
#define DJPEG_ERR_BAD_INTERLEAVE  0x0403
#define DJPEG_ERR_BAD_SUBSAMPLE   0x0404
#define DJPEG_ERR_BAD_COLOR_CONVERT 0x0405
#define DJPEG_ERR_BAD_CHANNELS    0x0406
#define DJPEG_ERR_BAD_411     0x0407
#define DJPEG_ERR_BAD_422     0x0410
#define DJPEG_ERR_BAD_411CONVERT  0x0411
#define DJPEG_ERR_BAD_422CONVERT  0x0412
#define DJPEG_ERR_BAD_CONVERT   0x0413
#define DJPEG_ERR_BAD_SWITCH    0x0414
#define DJPEG_ERR_BAD_4114      0x0415
#define DJPEG_ERR_BAD_4224      0x0416
#define DJPEG_ERR_BAD_4114CONVERT 0x0417
#define DJPEG_ERR_BAD_4224CONVERT 0x0420
#define DJPEG_ERR_BAD_111CONVERT  0x0421
#define DJPEG_ERR_BAD_1111CONVERT 0x0422

typedef struct _HUFFMAN_TREE
{                /* Huffman tree as suggested by JPEG */
    int mincode[8];             /* mincode[i]=min code with 8+i+1 bit length*/
    int maxcode[8];             /* maxcode[i]=max code with 8+i+1 bit length
                                   -1 if no code with this length*/
                                /* min/maxcode are inclusive and only the
                                   last 8-bit are stored in min/maxcode */
    int valptr[8];              /* (first index (of mincode) to the array
                                   'huffval' in DHUFF_TABLE,i.e Huffman
                                   value for codeword 'mincode') - mincode */
} HUFFMAN_TREE;

typedef struct _HUFFMAN_ELEM
{
    unsigned char codelen;      /* number of bits (1-8) in the code word,
                                   0 means >8 bits, use hufftree instead */
    unsigned char value;        /* Huffman value if codelen <= 8 */
    HUFFMAN_TREE *hufftree;     /* Huffman tree if codelen > 8 */
} HUFFMAN_ELEM;

typedef struct _DHUFF_TABLE
{
    int huff_class;              /* 0=DC table or lossless table, 1=AC table */
    int ident;                  /* Huffman table identifier (0 - 3) */
    HUFFMAN_ELEM huffelem[256]; /* Huffman Table */
    int huffval[256];           /* Huffman values for codes >8 bit long */
} DHUFF_TABLE;

typedef struct _QUANT_TABLE
{
    int precision;              /* 0 <-> 8-bit elements, 1 <-> 16-bit */
    int ident;                  /* table identifier (0 - 3) */
    int elements[80];           /* 64 table elements + extra 16 elements
                                   to catch error in Decode_AC */
} QUANT_TABLE;

typedef struct _FRAME_COMPONENT
{                /* Component-Spec structure */
    int ident;                  /* component identifier */
    int width;                  /* width of this component */
    int height;                 /* height of this component */
    int hsampling;              /* horizontal sampling factor */
    int vsampling;              /* vertical sampling factor */
    int quant_sel;              /* quantization table selector */
} FRAME_COMPONENT;

typedef struct _FRAME
{                /* Frame structure */
    int precision;              /* sample precision in bits */
    int width;                  /* width of the source image */
    int height;                 /* height of the source image */
    int ncomps;                 /* number of image components in frame */
    int horMCU;                 /* number of MCU in hor. direction */
    long totalMCU;              /* total number of MCU */
    FRAME_COMPONENT *comps;     /* array of 'ncomps' items */
} FRAME; 

#ifndef TILE_DATA_DEFINED
typedef struct {
  int width;    /* tile width */
  int height;   /* tile height */
  int components;   /* # of components: 1-4 */
  int *Hsamp, *Vsamp; /* one pair of <Hsamp,Vsamp> subsampling 
           factors per component */
  int iflag;    /* Interleave-flag: 
           1/0= Interleaved/Non-interleaved */
  unsigned char *data;  /* points to image/compressed data */
} TILE_DATA;

#define TILE_DATA_DEFINED
#endif

typedef struct _JPEGSUBTYPE
{
  unsigned char interleave_type;
  unsigned char chroma_subsample;
  unsigned char internal_colorconvert;
  unsigned char jpeg_tblselector;

  unsigned char horiz_subsample; /* left nibble of chroma_subsample unsigned char above */
  unsigned char vert_subsample;  /* right nibble of chroma_subsample unsigned char above */

} JPEGSUBTYPE;

typedef struct _DECODERSTRUCT
{
        int hSize, vSize; /* width/height of tile in pixels */
        int num_channels; /* bytes per pixel */

  int InternalUpsample; /* if TRUE, then upsampling is done in jpeg decoder */
  int InternalColor; /* if TRUE, then color conversion is done in jpeg decoder */

  /* next 6 bytes are a copy of jpegsubtype above */               
  unsigned char interleave_type;
  unsigned char chroma_subsample;
  unsigned char internal_colorconvert;  /* color conversion is needed */
  unsigned char jpeg_tblselector;

  unsigned char horiz_subsample; /* left nibble of chroma_subsample unsigned char above */
  unsigned char vert_subsample;  /* right nibble of chroma_subsample unsigned char above */

  int numb_huffman_pair_tables;
  DHUFF_TABLE *huffman_table_dc[4]; /* A maximum of 4 pairs of Huffman tables */
  DHUFF_TABLE *huffman_table_ac[4]; /* A maximum of 4 pairs of Huffman tables */
  int numb_quant_tables;
  QUANT_TABLE *quant_table[4];     /* A maximum of 4 quantization tables per tile */

  FRAME *frame;     /* A JPEG frame-structure */
  int *mcu_buf[4];
  unsigned char **row_bufs;
  int nrow_bufs;   /* number of row buffers */
  unsigned char *irow_bufs[16]; 

} DECODER_STRUCT, *DECODER;


 /* Prototypes */

/*
** You have to call dJPEG_DecodeInit() at least once before you call
** any other dJPEG routine. The argument to this routine is a handle
** to the decoder structure which is returned with default flags/values
** set in the structure it points to.
** save the pointer to decoder structure for later use in dJPEG routines.
** The default flags set in this Init function are to use the internal
** subsampling and internal color space rotation routines if need be.
** Default values set in this function are the tile size in pixels (64) 
** and the number of channels in use (3). 
** in case of successful return, the DJPEG_NO_ERROR value is returned.
*/
#ifdef __cplusplus
extern "C"
#endif
JPEGEXPORT
 int dJPEG_DecoderInit(void **decoder);

/*
** To free the decoder structure, we also provide the dJPEG_DecoderFree
** routine...dJPEG_DecoderFree() needs to be called only once after all tiles
** are processed, IF ENTROPY_ONLY case is our parse option. In that case, the
** second argument to this routine must be set to 1.
** If ENTROPY_AND_HEADER parse option is selected, the jpeg header is parsed
** for each tile being processed and hence new table space is allocated for
** each tile. In this case, we NEED TO CALL dJPEG_DecoderFree() FOR EVERY TILE
** with second argument set to zero. In this case all table spaces in decoder 
*  structure will be free, but the structure itself will not.
*/
#ifdef __cplusplus
extern "C"
#endif
JPEGEXPORT
void dJPEG_DecoderFree(void *decode, int freeStructure);

/*
** To disable the internal upsampling in order to use your own upsample routine,
** you can call dJPEG_DisableUpsample() after dJPEG_DecoderInit() has been
** called.
*/
#ifdef __cplusplus
extern "C"
#endif
JPEGEXPORT
 void dJPEG_DisableUpsample(void *decoder); 

/*
** To disable the internal color rotation from YCbCr to RGB,
** so as to use your own  routine,
** you can call dJPEG_DisableColorConvert() after dJPEG_DecoderInit() has been
** called.
*/
#ifdef __cplusplus
extern "C"
#endif
JPEGEXPORT
 void dJPEG_DisableColorConvert(void *decoder);

/*
** To enable the internal upsampling and hence use the one of the provided
** internal upsampling routines. Note that this mode is the default mode 
** of the decoder once dJPEG_DecoderInit() is called. Also note that unlike
** the similar encoder routine, we do not need to specify the type of
** upsampling enabled. That will be determined upon reading the tile header.
*/
#ifdef __cplusplus
extern "C"
#endif
JPEGEXPORT
 void dJPEG_EnableUpsample(void *decoder);

/*
** To enable the internal color rotation from YCbCr space to RGB space.
*/
#ifdef __cplusplus
extern "C"
#endif
JPEGEXPORT
 void dJPEG_EnableColorConvert(void *decoder);

/*
** To pass along the tile header information on the compression subtype to
** the dJPEG routines, we need to call the following routine before
** calling dJPEG_DecodeTile() on each tile.
** The jpeg subtype field must be an unsigned 32 bit variable, and is
** assumed to be compatible with the compression subtype field of the tile
** header entry defined in table IV.3 of the FlashPix Spec.
*/
#ifdef __cplusplus
extern "C"
#endif
JPEGEXPORT
 int dJPEG_CopyJpegSubtype(void *decoder, unsigned long JpegSubtype);


/*
** dJPEG_DecodeTile() is the routine that will actually decode a tile of
** data. The preliminary setup for properly calling this routine has been
** shown in the ReadMe file of this directory.
** the arguments are as follows:
** *outbuf : OUT   Pointer to the output buffer.
** outbuf_size: IN  Size of output buffer in bytes. For a tile of width and
**                  height of 64 pixels, with 3 color channels, this value is
**                  set as 64 * 64 *3.
** *inbuf: IN  Pointer to input data. This can be either just the entropy data
**             or the JPEG header stream used for this tile followed by the
**             tile's entropy data.
**             Currently (3/14/96) the former mode has not been completely 
**         debugged.
** inbuf_size: IN  Size of input buffer, which is read from tile header stream
**                 and in the case of jpeg header and entropy, is added by the
**                 size of the jpeg header in bytes.
** *decoder    IN  Void pointer to decoder's internal structure. This structure
**             holds flags to show whether internal upsampling and color
**             rotation are disabled or not. Also, it copies over the JPEG
**             compression subtype field info (interleave type, 
**       chroma subsample, internal color convert, table selector) 
**                 to be used in proper selection of sampling/rotation routines.
** parse_option IN  Will be either ENTROPY_ONLY or ENTROPY_AND_HEADER (defined
**        in decoder.h). 
** dct_method IN  Will be either CHEN_DCT, WINOGRAD, or PRUNED_WINOGRAD 
**        (defined in decoder.h). Currently PRUNED_WINOGRAD is not
**        completely tested.
*/
#ifdef __cplusplus
extern "C"
#endif
JPEGEXPORT
int  dJPEG_DecodeTile(unsigned char *outbuf,
         size_t outbuf_size,
         unsigned char *inbuf,
         size_t inbuf_size,
         void *decoder,
               int parse_option,  
         int dct_method 
         );

/*
** dJPEG_DecodeTileHeader() is the parser for the jpeg header stream.
** If the same jpeg header is to be used for many tiles, this routine can
** be called once after reading in the jpeg header stream, and the results of
** the parse will be stored in the decoder structure passed into the routine.
** The arguments have the same meanings as the ones above in dJPEG_DecodeTile().
**
** Note that although currently (3/14/96) this routine is functional, the
** testing of its use is not completed yet, and it is advised to bypass
** calling this function and instead use dJPEG_DecodeTile() routine
** with the ENTROPY_AND_HEADER parse option. In which case  dJPEG_DecodeTile()
** routine will parse the attached header for each tile.
*/ 
#ifdef __cplusplus
extern "C"
#endif
JPEGEXPORT
int dJPEG_DecodeTileHeader( unsigned char *inbuf,
                  size_t inbuf_size,
                  void *decoder,
                  int dct_method);


/*
** dJPEG_SetTileSize can be called if the tile size or number of 
** channels are different than the default values set in dJPEG_DecodeInit().
** in case of successful return, the DJPEG_NO_ERROR value is returned.
*/ 
#ifdef __cplusplus
extern "C"
#endif
JPEGEXPORT
int dJPEG_SetTileSize( void *decoder, 
           int hSize,  /* width of a tile in pixels */
           int Vsize,  /* height of a tile in pixels */ 
           int num_channels /* number of channels (bytes per pixel)*/
                      );


/*
** dJPEG_UpsampleAndConvert() routine will determine what upsampling routine
** to use (4:1:1 vs. 4:2:2 vs. no upsampling, a so-called 1:1:1 case) based
** on decoder structure values copied over from the compression subtype
** field of the tile. It will also decide whether color space rotation needs
** to be performed.
** This function is called from dJPEG_DecodeTile() and is not set up as an 
** external function to be called from other applications.
** in case of successful return, the DJPEG_NO_ERROR value is returned.
*/
JPEGEXPORT
int dJPEG_UpsampleAndConvert(DECODER_STRUCT *decoder, 
            unsigned char *dataBuffer,  
            size_t oneTile_inBytes);
 

#endif  /* _DECODER_H */
