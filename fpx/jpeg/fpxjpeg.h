//  ----------------------------------------------------------------------------
//  MODULE    : FlashPixJPEG.h 
//  LANGUAGE  : C 
//  CREATOR   : Yue Zhang 
//  CREAT. DATE : Thursday, March 28, 1996 
//  DESCRIPTION : This file contains declarations of JPEG functions called by 
//          FlashPix based on HP JPEG code. 
//  COMMENTS  : The functions calls include two parts, encoder and decoder.
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
  #ifndef FlashPixJPEG_h
  #define FlashPixJPEG_h
//  ----------------------------------------------------------------------------

//  Includes
//  --------
  #include "decoder.h"
  #include "enctile.h"
  #include "encoder.h"
  #include "public.h"

//  Constants
//  ---------
  /*#ifndef BYTE 
  #define BYTE  unsigned char
  #define LPBYTE  BYTE *
  #endif

  // Define decoder method
  #define ENTROPY_ONLY    0
  #define ENTROPY_AND_HEADER  1

  #define CHEN_DCT    0
  #define WINOGRAD    1
  #define PRUNED_WINOGRAD   2


//  Types Declarations from HP code enctile.h
//  ------------------------------------------
  typedef struct {
      unsigned short ehufcode[256];   // ehufcode[i]=Huffman code for value i 
      int ehufsize[256];              // number of bits in ehufcode[i]
  } HUFFMAN_TABLE;

  typedef struct {
    HUFFMAN_TABLE luminanceDCHuffman;
    HUFFMAN_TABLE luminanceACHuffman;
    HUFFMAN_TABLE chrominanceDCHuffman;
    HUFFMAN_TABLE chrominanceACHuffman;
    int luminanceQuantization[64];
    int chrominanceQuantization[64];
    int threshLuminanceQuantization[64];
    int threshChrominanceQuantization[64];
    int en_last_dc[4];
  }  JPEG_STRUCT;

  typedef struct {
    int width;     // tile width 
    int height;    // tile height 
    int components; // # of components: 1-4 
    int *Hsamp, *Vsamp;  // one pair of <Hsamp,Vsamp> subsampling factors per component 
    int iflag;     // Interleave-flag: 1/0= Interleaved/Non-interleaved 
    unsigned char *data; // points to image/compressed data 
  } TILE_DATA;

  typedef struct {
    BYTE      subsampling;  // stored in FPX tile header format
    int       iHsamp[4];    // same thing, but in the format the
    int       iVsamp[4];    // encoder prefers
    JPEG_STRUCT   jpegStruct;   // used by the encoder to store tables
    BYTE      ssDisabled;   // if non-zero, internal subsampling is
                    // disabled
    BYTE      YCrCbDisabled;  // if non-zero, internal YCrCb is disabled
    int       xPixels;    // x dimension of a tile in pixels
    int       yPixels;    // y dimension of a tile in pixels
    int       bytes;      // number of bytes per pixel
    int       qFactor;    // the quality level of the encoding
    TILE_DATA   tile_data;
    int       nu_huff;    // # of Huffman Tables (4, two DC-AC sets)
    int       nu_qtables;   // # of Q-tables (a max of 4 tables)
    unsigned char *scratch;   // place to hold rotated/subsampled data
    unsigned char *header;    // place to hold the JPEG header
    long      headerBytes;  // how big is the header anyway?
  } ENCODER_STRUCT;

  #define ENCODER ENCODER_STRUCT*


  //  Types Declarations from HP code decoder.h
  //  ------------------------------------------
  typedef struct _HUFFMAN_TREE
  {                       // Huffman tree as suggested by JPEG 
      int mincode[8];             // mincode[i]=min code with 8+i+1 bit length
      int maxcode[8];             // maxcode[i]=max code with 8+i+1 bit length
                                     -1 if no code with this length
                                  // min/maxcode are inclusive and only the
                                     last 8-bit are stored in min/maxcode 
      int valptr[8];              // (first index (of mincode) to the array
                                     'huffval' in HUFFMAN_TABLE,i.e Huffman
                                     value for codeword 'mincode') - mincode 
  } HUFFMAN_TREE;

  typedef struct _HUFFMAN_ELEM
  {
      unsigned char codelen;      // number of bits (1-8) in the code word,
                                     0 means >8 bits, use hufftree instead 
      unsigned char value;        // Huffman value if codelen <= 8 
      HUFFMAN_TREE *hufftree;     // Huffman tree if codelen > 8 
  } HUFFMAN_ELEM;

  typedef struct _HUFFMAN_TABLE
  {
      int huff_class;              // 0=DC table or lossless table, 1=AC table 
      int ident;                  // Huffman table identifier (0 - 3) 
      HUFFMAN_ELEM huffelem[256]; // Huffman Table 
      int huffval[256];           // Huffman values for codes >8 bit long 
  } DHUFF_TABLE;

  typedef struct _QUANT_TABLE
  {
      int precision;              // 0 <-> 8-bit elements, 1 <-> 16-bit 
      int ident;                  // table identifier (0 - 3) 
      int elements[80];           // 64 table elements + extra 16 elements
                                     to catch error in Decode_AC 
  } QUANT_TABLE;

  typedef struct _FRAME_COMPONENT
  {                // Component-Spec structure 
      int ident;                  // component identifier 
      int width;                  // width of this component 
      int height;                 // height of this component 
      int hsampling;              // horizontal sampling factor 
      int vsampling;              // vertical sampling factor 
      int quant_sel;              // quantization table selector 
  } FRAME_COMPONENT;

  typedef struct _FRAME
  {                // Frame structure 
      int precision;              // sample precision in bits 
      int width;                  // width of the source image 
      int height;                 // height of the source image 
      int ncomps;                 // number of image components in frame 
      int horMCU;                 // number of MCU in hor. direction 
      long totalMCU;              // total number of MCU 
      FRAME_COMPONENT *comps;     // array of 'ncomps' items 
  } FRAME; 

  typedef struct _DECODERSTRUCT
  {
          int hSize, vSize; // width/height of tile in pixels 
          int num_channels; // bytes per pixel 

    int InternalUpsample; // if TRUE, then upsampling is done in jpeg decoder 
    int InternalColor; // if TRUE, then color conversion is done in jpeg decoder 

    // next 6 bytes are a copy of jpegsubtype above                
    unsigned char interleave_type;
    unsigned char chroma_subsample;
    unsigned char internal_colorconvert;  // color conversion is needed 
    unsigned char jpeg_tblselector;

    unsigned char horiz_subsample; // left nibble of chroma_subsample unsigned char above 
    unsigned char vert_subsample;  // right nibble of chroma_subsample unsigned char above 

    int numb_huffman_pair_tables;
    DHUFF_TABLE *huffman_table_dc[4]; // A maximum of 4 pairs of Huffman tables 
    DHUFF_TABLE *huffman_table_ac[4]; // A maximum of 4 pairs of Huffman tables 
    int numb_quant_tables;
    QUANT_TABLE *quant_table[4];     // A maximum of 4 quantization tables per tile 

    FRAME *frame;     // A JPEG frame-structure 

  } DECODER_STRUCT;

  #define DECODER DECODER_STRUCT*


//  Types Definitions
//  -----------------

//  'extern' Functions
//  ------------------

//  Functions declarations
//  ----------------------

#ifdef __cplusplus
extern  "C" {
#endif 

    short     eJPEG_Init(ENCODER *pencoder);
  
    int     eJPEG_DisableInternalSubsampling(ENCODER encoder);
  
    int     eJPEG_DisableInternalYCbCr(ENCODER encoder);
  
    int     eJPEG_SetSubsampling(ENCODER encoder, BYTE subSampling); 

    int     eJPEG_SetQFactor(ENCODER encoder, int quality);
  
    int     eJPEG_SetJPEGTables(
          int cls[4],       
          int ident[4],     // for arbitrary channel/table pairings...
          int *qTables[4],
          int *huffACBits[4],
          int *huffACValues[4],
          int *huffDCBits[4],
          int *huffDCValues[4]
          );
          
    int     eJPEG_SetTileSize(
          ENCODER encoder,
          int hSize,      // in pixels, the width of a tile
          int vSize,      // in pixels, the height of a tile
          int bytesPerPixel // how many bytes per pixel
          );
          
    int     eJPEG_CreateHeader(
          ENCODER encoder,      // same value returned by eJPEG_Init
          long hdrBufferSize,     // the size of the <hdrBuffer> in bytes
          unsigned char *hdrBuffer, // the buffer itself
          long *hdrBufferUsed     // upon return shows the amount of
                        // <hdrBuffer> that was used
          );
          
    int     eJPEG_ConcatenateHeader(ENCODER encoder);

    long    eJPEG_EncodeTile(
          ENCODER encoder,      // same value returned by eJPEG_Init
          unsigned char *inbuf, // assumed to be the size of a tile or sub-
                      // sampled tile!
          unsigned char *outbuf,  // the buffer to put the compressed tile into
          size_t outbuf_size    // size of the output buffer
          );

    int     eJPEG_Shutdown(ENCODER encoder);
        
    int     dJPEG_DecoderInit(DECODER* pDecoder);

    void    dJPEG_DisableColorConvert(DECODER decoder); 

    void    dJPEG_EnableColorConvert(DECODER decoder);
    
    void    dJPEG_DisableUpsample(DECODER decoder);
    
    void    dJPEG_EnableUpsample(DECODER decoder); 
    
    void    dJPEG_DecoderFree(DECODER decoder); 
    
    int     dJPEG_CopyJpegSubtype(DECODER decoder, unsigned long JpegSubtype);
    
    int     dJPEG_DecodeTile(unsigned char *outbuf,
               size_t outbuf_size,
               unsigned char *inbuf,
               size_t inbuf_size,
               DECODER decoder,
                     int parse_option, 
               int dct_method);
    
    int     dJPEG_DecodeTileHeader(unsigned char *inbuf,
               size_t inbuf_size,
               DECODER decoder,
              int dct_method); 
              
    int     dJPEG_SetTileSize( void *decoder, 
                 int hSize,  
                 int vSize,  
                 int num_channels);
                 
#ifdef __cplusplus
} // end extern
#endif 

//  'extern' Variables
//  ------------------
*/
//  ----------------------------------------------------------------------------
  #endif // FlashPixJPEG_h
//  ----------------------------------------------------------------------------
