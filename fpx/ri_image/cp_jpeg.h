//  ----------------------------------------------------------------------------
//  MODULE    : CompressorJPEGSS
//  LANGUAGE  : C++
//  CREATOR   : Yue Zhang 
//  CREAT. DATE : Monday, March 25, 1996 
//  DESCRIPTION :  
//  COMMENTS  : 
//      SCCSID        : @(#)cp_jpeg.h 1.2 12:41:18 08 Jul 1997
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
  #ifndef CompressorJPEG_h
  #define CompressorJPEG_h
  #ifndef Commun_h
    #include  "common.h"
  #endif
//  ----------------------------------------------------------------------------

//  Includes
//  --------

  #include  "olecomm.h"
  #include  "fpxjpeg.h"
    #include  "buffdesc.h"
  #include  "fpxcompress.h"
  
//  Constants
//  ---------
  // Default JPEG header size
  #define DEFAULTJPEGHEADERSIZE 2048
  
//  Types Declarations
//  ------------------
    
//  Types Definitions
//  -----------------

//  Classes Declarations
//  --------------------

  class PCompressorJPEG;
  
  class PCompressorSinColor;


//  Classes Definitions
//  -------------------

  // Class for JPEG compression/decompression
  class PCompressorJPEG : public obj_Compresseur
  {
  
  public:
                   PCompressorJPEG();
                  ~PCompressorJPEG();

        // Return the encoder 
          ENCODER     GetEncoder()  { return pEncoder; }
        
        // Return the decoder 
          DECODER     GetDecoder()  { return pDecoder; }
        
        // Return the jpeg header buffer 
          unsigned char*  GetHeaderBuf() { return jpegHeader; }
            
        // Return the size of jpeg header 
          unsigned long GetHeaderSize() { return headerSize; }
        
        // Set parameters for jpeg compression.
            int     SetCompressionParameters( unsigned char theInterleaveType,
                      unsigned char theChromaSubSample,
                      unsigned char theInternalColorConv,
                      long   compressionSubtype,
                      unsigned char theQualityFactor,
                      unsigned short theBytesPerPixel,
                      unsigned char* theJpedHeader,
                      unsigned long   theHeaderSize, 
                      Boolean     selector);  
                          
        // Compress an image. Return false on error.
            int     Compress(unsigned char *  uncompressedData, 
                      short width, short height,
                        unsigned char ** pcompressedData, 
                        long* sizeCompressed); 
                    
        // Uncompress an image. Return false on error.
            int     Decompress(unsigned char* uncompressedData, 
                      short width, short height, 
                      unsigned char* compressedData, 
                      long sizeCompressed);
                    
        // Allocate buffer for compressor and decompressor
            Boolean   AllocBuffer (unsigned long size); 
            
  protected:
        unsigned char*    dataBuffer; 
        unsigned long     sizeOfBuffer; 
        
        unsigned short    bytesPerPixel; 
        
        ENCODER       pEncoder; 
        DECODER       pDecoder; 
        
        Boolean       decoderHeaderPresent;
        Boolean       encoderHeaderPresent;
        unsigned char     jpegHeader[DEFAULTJPEGHEADERSIZE]; 
        unsigned long     headerSize; 
                
        unsigned char     interleaveType;     // Interleave type (1 for non-interleave, 
                              // 0 for interleave)
        unsigned char     chromaSubSample;    // Subsampling ( 1 for subsample, 
                              // 0 for non-subsample)
        unsigned char     internalColorConv;    // Internal color convertion ( 1 for conversion, 
                              // 0 and non-conversion)
        unsigned char   qualityFactor;      // Quality factor
    
        unsigned char   jpegTableIndex;     // The jpeg table index
  };

  // Class for single color compression/decompression
  class PCompressorSinColor : public obj_Compresseur
  {
  
  public:
                   PCompressorSinColor();
                  ~PCompressorSinColor();
                  
            // Set parameters for single color compression.
            void  SetCompressionParameters( short numChannels); 
                          
            Boolean   Compress(unsigned char *  uncompressedData, 
                      short width, short height,
                        unsigned char ** pcompressedData, 
                        long* sizeCompressed); 
                    
            Boolean   Decompress(unsigned char* uncompressedData, 
                      short width, short height, 
                      unsigned char* compressedData, 
                      long sizeCompressed);
  
  protected:
        unsigned char*    dataBuffer; 
        Boolean       dataBufPresent;
        unsigned short    bytesPerPixel; 
                    
  }; 


//  'inline' Functions
//  ------------------

//  'extern' Functions
//  ------------------

int jpegErrorToFPXerror( long djpegError);

//  'extern' Variables
//  ------------------

//  ----------------------------------------------------------------------------
  #endif // CompressorJPEG_h
//  ----------------------------------------------------------------------------
