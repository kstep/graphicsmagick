//  ----------------------------------------------------------------------------
//  MODULE    : CompressorJPEG.cpp
//  LANGUAGE  : C++ 
//  CREATOR   : Yue Zhang 
//  CREAT. DATE : Monday, March 25, 1996 
//  DESCRIPTION :  
//  COMMENTS  : 
//      SCCSID        : @(#)cp_jpeg.cpp 1.2 12:41:07 08 Jul 1997
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
  #ifdef macintosh
    #pragma segment CompressorJPEG
  #endif
//  ----------------------------------------------------------------------------

//  ----------------------------------------------------------------------------

//  Includes
//  --------

  #include  "debug.h"
  #include  "common.h"
#ifndef Memoire_h
  #include  "b_memory.h"
#endif
#ifndef FlashPixUtils_h
  #include  "fpxutils.h"
#endif
    #include  "ejpeg.h"     // CHG_FILE_ERR - needed for error code mapping
    #include  "cp_jpeg.h"

//  Constants
//  ---------

//  Variables
//  ---------

//  ----------------------------------------------------------------------------
//  Internal Functions
//  ----------------------------------------------------------------------------

//  ----------------------------------------------------------------------------
//  Member Functions
//  ----------------------------------------------------------------------------

PCompressorJPEG::PCompressorJPEG ()
{
  int status = FPX_OK;      // CHG_FILE_ERR - init to no error
  
  // At the begining, there is no encoder and decoder
  compresseurPresent  = decompresseurPresent = FALSE;
  
  // At the begining, there is no header for encoder and decoder
  encoderHeaderPresent = decoderHeaderPresent = FALSE;
  
  // Initialize header size
  headerSize = 0; 
  
  // At the begining, data buffer is not allocated
  dataBuffer    = NULL;
  sizeOfBuffer  = 0; 
  
  // Assume pixel is 24-bit
  bytesPerPixel = 3; 
  
  // Initialize compression options
  qualityFactor     = 20; 
  interleaveType    = 0;
  chromaSubSample   = 0;
  internalColorConv   = 0;
  
  // Create a encoder
  status = eJPEG_Init((void**)&pEncoder); 
  
  if ( !status ) {
    eJPEG_CreateHeader(pEncoder, DEFAULTJPEGHEADERSIZE, jpegHeader, (long *)&headerSize); 
    compresseurPresent = TRUE;
  }
      
  // Create a decoder
  status = dJPEG_DecoderInit((void**)&pDecoder);
  
  if ( !status )  {
    decompresseurPresent = TRUE;
    dJPEG_DecodeTileHeader(jpegHeader, headerSize, pDecoder, CHEN_DCT);
  }
}

PCompressorJPEG::~PCompressorJPEG()
{
  // Delete data buffer if it exists
  if ( dataBuffer )
    delete dataBuffer; 
    
  // Delete encoder and decoder if exist
  if ( compresseurPresent ) 
    eJPEG_Shutdown(pEncoder);

  if ( decompresseurPresent )
    dJPEG_DecoderFree(pDecoder, TRUE);
  
}

// Set parameters for jpeg compression.
int PCompressorJPEG::SetCompressionParameters( unsigned char theInterleaveType,
                  unsigned char   theChromaSubSample,
                  unsigned char   theInternalColorConv,
                  long      compressionSubtype,
                  unsigned char   theQualityFactor,
                  unsigned short  theBytesPerPixel,
                  unsigned char*  theJpedHeader,
                  unsigned long   theHeaderSize, 
                  Boolean     selector) 
{
  int status = FPX_OK;      // CHG_FILE_ERR - init to no error
  
  // For encoder we use internal header, so headerSize is zero
  if ( selector ) {

    interleaveType    = theInterleaveType;
    chromaSubSample   = theChromaSubSample;
    internalColorConv   = theInternalColorConv;
    bytesPerPixel     = theBytesPerPixel;
    jpegTableIndex    = GET_JPEGTablesIndex(compressionSubtype); 
    
    // Create header for decoder 
    memcpy(jpegHeader, theJpedHeader, theHeaderSize); 
    headerSize = theHeaderSize; 
    
    // Set subsampling option
    switch(chromaSubSample)
    {
      // no subsampling
      case 0:
      case 0x11:
        dJPEG_DisableUpsample(pDecoder); 
        break; 
         
      // 411 or 422 subsampling
      case 0x21:
      case 0x22:
        dJPEG_EnableUpsample(pDecoder); 
        break; 
         
      default:
        break;
    }
    
    // Set interleave type (1 for non-interleave and 0 for interleave)
    if ( interleaveType ) 
      pDecoder->interleave_type = TRUE;
    else 
      pDecoder->interleave_type = FALSE;
    
    // Set internal color conversion
    if ( !internalColorConv ) 
      dJPEG_DisableColorConvert(pDecoder); 
    else 
      dJPEG_EnableColorConvert(pDecoder); 
      
    // Set the tile size and number of channel
    status = dJPEG_SetTileSize(pDecoder, 64, 64, bytesPerPixel); 
    if (status) 
      return status; 
    
    // Set jpeg subtype
    status = dJPEG_CopyJpegSubtype(pDecoder, compressionSubtype);   
    
    decoderHeaderPresent = TRUE; 
  }
  else  
    if (  !encoderHeaderPresent   || 
        theQualityFactor  != qualityFactor    || 
        theChromaSubSample  != chromaSubSample    || 
        theInternalColorConv != internalColorConv   || 
        theBytesPerPixel != bytesPerPixel ) 
    {
      // Set new compression options
      interleaveType    = theInterleaveType;
      chromaSubSample   = theChromaSubSample;
      internalColorConv   = theInternalColorConv;
      bytesPerPixel     = theBytesPerPixel;
      jpegTableIndex    = GET_JPEGTablesIndex(compressionSubtype); 

      // Set new quality factor
      qualityFactor = theQualityFactor;   
      status = eJPEG_SetQFactor(pEncoder, qualityFactor);   
      if (status) 
        return status; 
      
      // Set subsampling option
      switch(chromaSubSample)
      {
        // no subsampling
        case 0:
        case 0x11: 
        {
          status = eJPEG_DisableInternalSubsampling(pEncoder);  
          if (status) 
            return status; 
          break; 
        }  
        // 411 or 422 subsampling
        case 0x21:
        case 0x22:
        {
          status = eJPEG_SetSubsampling(pEncoder, chromaSubSample); 
          if (status) 
            return status; 
          pEncoder->ssDisabled = FALSE;
          break; 
        }  
        default:
          break;
      }
      
      // Set interleave type ( 0 for interleave and 1 for non_interleave)
      if ( !interleaveType ) {
        status = eJPEG_EnableChannelInterleave(pEncoder);
        if (status) 
          return status; 
      }
      else {
        status = eJPEG_DisableChannelInterleave(pEncoder);  
        if (status) 
          return status; 
      }

      // Set internal color conversion ( 0 for no color conversion and 1 for yes)
      if ( !internalColorConv ) {
        status = eJPEG_DisableInternalYCbCr(pEncoder);  
        if (status) 
          return status; 
      }
      else {
        status = eJPEG_EnableInternalYCbCr(pEncoder); 
        if (status) 
          return status; 
      }
      
      // Set the tile size and number of channel
      status = eJPEG_SetTileSize(pEncoder, 64, 64, bytesPerPixel);  
      if (status) 
        return status; 
      
      // Create a jpeg header
      if ( jpegTableIndex )
        status = eJPEG_CreateHeader(pEncoder, DEFAULTJPEGHEADERSIZE, jpegHeader, (long *)&headerSize);  
      else
        status = eJPEG_ConcatenateHeader(pEncoder, jpegHeader, (long *)&headerSize);  
      
      if (status) 
        return status; 

      encoderHeaderPresent = TRUE; 
    }

  return status; 
}

// Compress the data. Return false if error.
int PCompressorJPEG::Compress(unsigned char * uncompressedData, 
                  short width, short height,
                    unsigned char ** pcompressedData, 
                    long* sizeCompressed)
{
  int status = FPX_OK;      // CHG_FILE_ERR - init to no error
  register long   numberofpixel = width * height;
  register long   buflen = numberofpixel * bytesPerPixel;

  // Verify the parameters
  assert(uncompressedData != NULL);

  // Set the tile size and number of channel
  status = eJPEG_SetTileSize(pEncoder, width, height, bytesPerPixel); 
  if (status) 
    return status; 
  
  // Allocate two data buffers 
  if ( !AllocBuffer( buflen + DEFAULTJPEGHEADERSIZE ) )
    return eJPEG_MEMORY_ERROR; 
    
  // Perform JPEG compression
  *sizeCompressed = eJPEG_EncodeTile(pEncoder, uncompressedData, dataBuffer, buflen); 

  // Set the compressed data buffer
  *pcompressedData = dataBuffer; 
  
  if ( *sizeCompressed <= 0)  
    status = eJPEG_MEMORY_ERROR;    // CHG_FILE_ERR - inBuf was < outBuf in size
    
  return status;
}

// Decompress data.
int PCompressorJPEG::Decompress(unsigned char* uncompressedData, 
                    short width, short height, 
                    unsigned char* compressedData, 
                    long sizeCompressed)
{
  int       status = FPX_OK;      // CHG_FILE_ERR - init to no error
  register long   numberofpixel = width * height;
  register long   buflen = numberofpixel * bytesPerPixel;

  // Verify the parameters
  assert(uncompressedData != NULL);

  // Allocate the buffer for decompressed data
  if ( !AllocBuffer( buflen + DEFAULTJPEGHEADERSIZE ) )
    return DJPEG_ERR_ALLOC_DECODER; 
  
  // Set the tile size and number of channel
  status = dJPEG_SetTileSize(pDecoder, width, height, bytesPerPixel); 
  if (status) 
    return status; 
  
  // Concatenate compressed data with header if jpeg table index is 0
  if ( jpegTableIndex ) {
    memcpy(dataBuffer, jpegHeader, headerSize);   
    memcpy(dataBuffer + headerSize, compressedData, sizeCompressed);  

    // Perform JPEG decompression
    status = dJPEG_DecodeTile(uncompressedData, buflen, dataBuffer, sizeCompressed + headerSize, 
              pDecoder, ENTROPY_AND_HEADER, WINOGRAD);                
  }
  else {
    memcpy(dataBuffer, compressedData, sizeCompressed);   
      
    // Perform JPEG decompression
    status = dJPEG_DecodeTile(uncompressedData, buflen, dataBuffer, sizeCompressed, 
              pDecoder, ENTROPY_AND_HEADER, WINOGRAD);                
  }
  
  // Perform JPEG decompression
  /*status = dJPEG_DecodeTile(uncompressedData, buflen, compressedData, sizeCompressed, 
            pDecoder, ENTROPY_ONLY, WINOGRAD);  */
  dJPEG_DecoderFree(pDecoder, FALSE);
  
  return status;
}

// Allocate two buffers for compressor and decompressor
Boolean PCompressorJPEG::AllocBuffer ( unsigned long size )
{
  // If the buffer previously allocated is too small
  if (size > sizeOfBuffer) {
  
    // Free the old buffer
    FastDeleteArray(dataBuffer, unsigned char);
    
    // Allocate the buffer
    FastAllocArray(dataBuffer, unsigned char, size);
        
    if ( dataBuffer ) {
      sizeOfBuffer = size;
      return TRUE; 
    }
    else {
      sizeOfBuffer = 0;
      return FALSE; 
    } 
  }
  else 
    return TRUE; 
}


PCompressorSinColor::PCompressorSinColor ()
{
  // At the begining, data buffer is not allocated
  dataBufPresent = FALSE;
  
  // Set default channels to be 3 
  bytesPerPixel = 3; 
}

PCompressorSinColor::~PCompressorSinColor()
{
  // Delete data buffer if it exists
  if ( dataBufPresent )
    delete dataBuffer; 
}

void PCompressorSinColor::SetCompressionParameters(short numChannels)
{
  bytesPerPixel = numChannels; 
}

Boolean PCompressorSinColor::Compress(unsigned char *  uncompressedData, 
                      short width, short height,
                        unsigned char ** pcompressedData, 
                        long* sizeCompressed)
{
  // Allocate the data buffer if it doesn't exist
  if ( !dataBufPresent ) {
    dataBuffer = new unsigned char[sizeof(Pixel)]; 
    if (dataBuffer==NULL) {
      return FALSE;
    }
    dataBufPresent = TRUE; 
  }
  
  if ( bytesPerPixel == 4 )
    memcpy(dataBuffer, uncompressedData, bytesPerPixel); 
  else
    memcpy(dataBuffer, uncompressedData + 1, bytesPerPixel); 
  
  *pcompressedData = dataBuffer; 
  
  // Set the compressed data size 
  *sizeCompressed = bytesPerPixel; 
    
  return TRUE; 
}

Boolean PCompressorSinColor::Decompress(unsigned char* uncompressedData, 
                      short width, short height, 
                      unsigned char* compressedData, 
                      long sizeCompressed)
{
  register long   numberofpixel = width * height;
  unsigned char alphaValue = 0; 
  
  if ( sizeCompressed == 4 ) {
    alphaValue = compressedData[0]; 
    compressedData++; 
  }
  
  // Rebuild the pixel buffer
  for ( register long i= 0; i< numberofpixel; i++ )
  {
    uncompressedData[4*i] = alphaValue; 
    uncompressedData[4*i + 1] = compressedData[0]; 
    uncompressedData[4*i + 2] = compressedData[1]; 
    uncompressedData[4*i + 3] = compressedData[2]; 
  }
  
  return TRUE; 
}


//  ----------------------------------------------------------------------------
//  External Functions
//  ----------------------------------------------------------------------------

// CHG_FILE_ERR - the following maps an error value returned by the JPEG subsystem
//        into a similar FPXStatus value. 

int jpegErrorToFPXerror( long djpegError)
{
  FPXStatus err = FPX_INVALID_COMPRESSION_ERROR;
  
  switch( djpegError) {
    case eJPEG_NO_ERROR:
//    case DJPEG_NO_ERROR:
      err = FPX_OK;
      break;
      
    case DJPEG_ERR_ALLOC_DECODER:         // These are errors from decoder
      err = FPX_MEMORY_ALLOCATION_FAILED;
      break;
    case DJPEG_ERR_BAD_INTERLEAVE:
    case DJPEG_ERR_BAD_SUBSAMPLE:
      err = FPX_INVALID_PIXEL_FORMAT;
      break;
    case DJPEG_ERR_BAD_COLOR_CONVERT:
      err = FPX_COLOR_CONVERSION_ERROR;
      break;
    case DJPEG_ERR_BAD_CHANNELS:
      break;
    case DJPEG_ERR_BAD_411:
    case DJPEG_ERR_BAD_422:
      break;
    case DJPEG_ERR_BAD_411CONVERT:
    case DJPEG_ERR_BAD_422CONVERT:
    case DJPEG_ERR_BAD_CONVERT:
      err = FPX_COLOR_CONVERSION_ERROR;
      break;
    case DJPEG_ERR_BAD_SWITCH:
      break;
    case DJPEG_ERR_BAD_4114:
    case DJPEG_ERR_BAD_4224:
      break;
    case DJPEG_ERR_BAD_4114CONVERT:
    case DJPEG_ERR_BAD_4224CONVERT:
    case DJPEG_ERR_BAD_111CONVERT:
    case DJPEG_ERR_BAD_1111CONVERT:
      err = FPX_COLOR_CONVERSION_ERROR;
      break;

    case eJPEG_SUBSAMPLING_NOT_SUPPORTED:     // These are errors from encoder
      break;
    case eJPEG_INVALID_ENCODER:
      break;
    case eJPEG_UNSUPPORTED_SUBSAMPLING:
    case eJPEG_UNSUPPORTED_BYTES_PER_PIXEL:
      err = FPX_INVALID_PIXEL_FORMAT;
      break;
    case eJPEG_MEMORY_ERROR:
      err = FPX_MEMORY_ALLOCATION_FAILED;
      break;
    case eJPEG_BAD_HUFFMAN_TABLE:
    case eJPEG_BAD_QUANT_TABLE:
      err = FPX_INVALID_JPEG_TABLE;
      break;

    case EJPEG_ERROR_PARSER:            // These are errors from encoder
      break;
    case EJPEG_ERROR_MEM:
      err = FPX_MEMORY_ALLOCATION_FAILED;
      break;
    case EJPEG_ERROR_EOF:
      break;
    case EJPEG_ERROR_FORMAT:
      err =FPX_INVALID_FORMAT_ERROR;
      break;
  }
  return err;
}

//  - EOF ----------------------------------------------------------------------
