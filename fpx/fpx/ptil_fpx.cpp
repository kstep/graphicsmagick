//  ----------------------------------------------------------------------------
//  MODULE    : PTileFlashPix
//  LANGUAGE  : C++ 
//  CREATOR   : Philippe BOSSUT 
//  CREAT. DATE : Wednesday, March 20, 1996 
//  DESCRIPTION :  
//  COMMENTS  : 
//      SCCSID      : @(#)ptil_fpx.cpp  1.11 12:24:18 08 Jul 1997
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
#ifdef _WINDOWS
  #pragma code_seg ("PTileFlashPix")
#else
  #pragma segment PTileFlashPix
#endif

//  ----------------------------------------------------------------------------
  #include "ptil_fpx.h"
//  ----------------------------------------------------------------------------

//  Includes
//  --------

#ifndef Memoire_h
  #include  "b_memory.h"
#endif

#ifndef Debug_h
  #include  "debug.h"
#endif

#ifndef FPXBaselineIO_h
  #include "fpxlibio.h"
#endif

#ifndef FPXBaselineView_h
  #include "fpxlib.h"
#endif


#ifndef OLECommun_h
  #include "olecomm.h"
#endif

#ifndef OLECore_h
  #include "olecore.h"
#endif

#ifndef OLEStorages_h
  #include  "olestorg.h"
#endif

#ifndef OLEHeaderStream_h
  #include  "olehstrm.h"
#endif

#ifndef OLEProperties_h
  #include  "oleprop.h"
#endif

#ifndef OLEPropertySet_h
  #include  "oleprops.h"
#endif

#ifndef OLEFiles_h
  #include  "olefiles.h"
#endif


#ifndef FlashPixUtils_h
  #include "fpxutils.h"
#endif


#ifndef AdvancedIVUE_h
  #include "viewimg.h"
#endif

#ifndef ColorTwist_h
  #include "coltwist.h"
#endif


#ifndef FileFlashPixIO_h
  #include "f_fpxio.h"
#endif

#ifndef PResolutionFlashPix_h
  #include "pres_fpx.h"
#endif

#ifndef FlashPixFormat_h
  #include "fpxformt.h"
#endif

#ifndef CompressorJPEG_h
    #include  "cp_jpeg.h"
#endif
#ifndef Compresseur32Vers24_h
    #include  "cp32to24.h"
#endif

#ifndef SwapBytes_h
  #include "swapbyte.h"
#endif

//  Constants
//  ---------

//  Variables
//  ---------
Boolean     gContrastVal = 0;
unsigned char gContrastLut[256];

//  ----------------------------------------------------------------------------
//  Internal Functions
//  ----------------------------------------------------------------------------

//  ----------------------------------------------------------------------------
//  Member Functions
//  ----------------------------------------------------------------------------

//  ----------------------------------------------------------------------------
//  Methods of the "PTileFlashPix" class
//
//  Manage a tile of a sub-image (generaly a tile is 256 x 256 pixels)
//  ----------------------------------------------------------------------------

PTileFlashPix::PTileFlashPix()
{
  pixelsSpace     = NON_AUTHORIZED_SPACE;
  rawPixelsSpace    = NON_AUTHORIZED_SPACE;
  dirtyCount      = -1;         // This value set when pixels read
  tileInitialize    = FALSE; 
  viewingParamApplied = FALSE;

  //Initialize compression options
  compression     = 0; 
  compressionSubtype  = 0;
  idCodec       = ConvertCompressionOption();
}


//  ----------------------------------------------------------------------------
// suppress the 'pixels' buffer from the list of buffer in memory and delete it
PTileFlashPix::~PTileFlashPix()
{
}


//  ----------------------------------------------------------------------------
// Initialize a tile with file assignment
// Use this constructor in Read mode
void PTileFlashPix::InitializeRead (PResolutionLevel* father, long pixelOffset, 
          long sizetile, long id, long theCompression, long theCompressionSubtype)
{
  // Call the parent method
  PTile::InitializeRead (father, pixelOffset, sizetile, id, theCompression);    

  // Set the compression type and subtype for tile
  compression     = theCompression; 
  compressionSubtype  = theCompressionSubtype;
  idCodec       = ConvertCompressionOption();
  
  // Set quality factor
  qualityFactor = ((PResolutionFlashPix *)fatherSubImage)->qualityFactor;       

  // Set the number of channels
  nbChannels = ((PResolutionFlashPix *)fatherSubImage)->nbChannels;         

  // Will have to expand the channels onto 32 bits per pixel if no compression
  if ((idCodec == TLC_Aucun) && (((PResolutionFlashPix*)fatherSubImage)->nbChannels != 4))
    idCodec = TLC_32Vers24;
}


//  ----------------------------------------------------------------------------
// Convert the FPX compression code to index of compressor 
TLC_IdCodec PTileFlashPix::ConvertCompressionOption ()
{
  TLC_IdCodec index;
  

  switch ((FPXCompressionOption)compression) {
    case NONE:
        index = TLC_Aucun;
      break;
      case SINGLE_COLOR:
        index = TLC_SingleColor;
      break;
      case JPEG_UNSPECIFIED:
      case JPEG_BY_QUALITY:
      case JPEG_BY_TABLE_GROUP:
        index = TLC_JPEG;
      break;
      
    // For FlashPix, currently assume valid compressions are only these above modes, anything else is 
    // considered invalid and no compressur for it  

    default:
        index = TLC_INVALID;
      break;
  }
  return index;
}


//  ----------------------------------------------------------------------------
// Write the data passed as parameters directly in the file
// The passed tile data is written to the file, no compression is performed.
// Returns: FPX_FILE_WRITE_ERROR
//
FPXStatus PTileFlashPix::WriteRawTile (FPXCompressionOption   compressOption, 
                    unsigned char     compressQuality,
                  long          compressSubtype,
                    unsigned long     dataLength,
                    void*         data)
{
  register long   TILE_WIDTH = fatherSubImage->fatherFile->tileWidth;

  Boolean   wasLocked, ok;
  FPXStatus   status = FPX_OK;


  assert(data);     

  // Set the number of channels
  nbChannels = ((PResolutionFlashPix *)fatherSubImage)->nbChannels;       

  wasLocked = IsLocked();
  Lock(); 
  PFlashPixFile* fileFPX = (PFlashPixFile*)(fatherSubImage->fatherFile->filePtr);
  OLEHeaderStream *subStreamData = ((PResolutionFlashPix*)fatherSubImage)->GetSubStreamData();

  // If the tile has never been written, reset the compression with the one passed in parameter
  if ( posPixelFic < 0 && !tileInitialize ) {
    compression       = compressOption; 
    idCodec         = ConvertCompressionOption();
    qualityFactor     = compressQuality;
    compressionSubtype  = compressSubtype;
  }

  // TEMPORARY IN THIS VERSION: when no compression, should compress 32->nbChannel
  if ((idCodec == TLC_Aucun) && (!fatherSubImage->fatherFile->existAlphaChannel))
    idCodec = TLC_32Vers24;

  // Compute position of the tile in file
  // CAUTION : go to the end of the file if the new tile is bigger than the old one
  // NO GARBAGE COLLECTION WITHIN THE FILE

  if ((posPixelFic < 0) || (tileSize < dataLength)) {
    ok = subStreamData->GetEndOfFile(&posPixelFic);
    posPixelFic;
  }

  if (ok) {
    // Set the tilesize as size of compressed data
    tileSize = dataLength;

    // Go to the right position in the stream...
    ok = subStreamData->Seek(posPixelFic);

    // ... then write the pixels or the compressed tile (at last...)
    if (ok)
      if (subStreamData->Write((unsigned char *)data,tileSize) == 0)
        ok = false;
  }

  if (!ok)
    status = FPX_FILE_WRITE_ERROR;
  
  // Clean up tile and release memory
  if (wasLocked == false)
    UnLock();

  if (ok) {
    // Update the global modification flag
    PFileFlashPixIO* father = (PFileFlashPixIO*)(fatherSubImage->fatherFile);
    father->tilesHasBeenModified = TRUE;
  }
  return status;
}


// --------------------------------------------------------------------------
// Write the tile in 'rawPixels' to the disk file. Use compression if necessary. 
//
// Returns: FPX_MEMORY_ALLOCATION_FAILED
//      FPX_ERROR           - if no 'rawPixels' buffer exists in memory
//      FPX_FILE_WRITE_ERROR
//      FPX_INVALID_COMPRESSION_ERROR - if no decompressor available
//
FPXStatus PTileFlashPix::Write()
{
  register long   TILE_WIDTH = fatherSubImage->fatherFile->tileWidth;
  long      sizeCompressed;       // Size of data after compression

  ptr_Compresseur monCompresseur; 

  char      *buffer = NULL;
  Pixel     *entireTile;
  Boolean     wasLocked;
  FPXStatus     status = FPX_OK;
  
  FPXBaselineColorSpace base = ((PResolutionFlashPix*)fatherSubImage)->baseSpace;
  FPXBaselineColorSpace used = ((PFileFlashPixIO*)fatherSubImage->fatherFile)->usedSpace;

  // Set the number of channels
  nbChannels = ((PResolutionFlashPix *)fatherSubImage)->nbChannels;       

  if( rawPixels == NULL) {
    assert( rawPixels);
    return FPX_ERROR;
  }

  // Allocate an intermediate tile if we have to pad or convert the tile
  if ((width < TILE_WIDTH)  || (height < TILE_WIDTH) || (used != base)) {
    FastCallocArray( entireTile, Pixel, TILE_WIDTH * TILE_WIDTH);

    if (entireTile == NULL)
      return FPX_MEMORY_ALLOCATION_FAILED;

    short i, j;
    Pixel *source = rawPixels;
    Pixel *dest   = entireTile;

    // Copy line by line and pad (padding conform to spec p.22)
    for (i = 0; i < height; i++, source+=width, dest+=TILE_WIDTH) {
      // Copy the raw pixels of the row
      memcpy (dest, source, width*sizeof(Pixel));

      // Pad the rest of the line with the value of the last pixel
      Pixel *pt = dest + width;
      Pixel padPixel = *(source + width - 1);

      for (j = width; j < TILE_WIDTH; j++, pt++)
        *pt = padPixel;
    }
    
    // Pad the remaining lines with a copy of the last line
    Pixel *pt = dest - TILE_WIDTH;
    
    for (i = height; i < TILE_WIDTH; i++, dest+=TILE_WIDTH)
      memcpy (dest, pt, TILE_WIDTH*sizeof(Pixel));
  } else
    entireTile = rawPixels;

  wasLocked = IsLocked();
  Lock(); 
  
  PFlashPixFile* fileFPX = (PFlashPixFile*)(fatherSubImage->fatherFile->filePtr);
  OLEHeaderStream *subStreamData = ((PResolutionFlashPix*)fatherSubImage)->GetSubStreamData();

// PTCH_104 - colorspace conversion is now in PTile::WriteRectangle()
//  if (used != base)
//  {
//    ConvertPixelBuffer((unsigned char*)(entireTile),TILE_WIDTH*TILE_WIDTH,used,base);
//    pixelsSpace = base;
//  }

  // If the tile has never been written, reset the compression with the one of the file
  if ( (posPixelFic < 0) && !tileInitialize ) {

    compression   = (long)((PResolutionFlashPix *)fatherSubImage)->compression; 
    idCodec     = ConvertCompressionOption();

    // Set quality factor
    qualityFactor = ((PResolutionFlashPix *)fatherSubImage)->qualityFactor;
  }

  // TEMPORARY IN THIS VERSION: single color compression is performed on tiles which consists
  // of only one color
  Pixel singleColorPixel = fatherSubImage->fatherFile->backgroundBase;    // default color value of this tile

  if ( (compression == SINGLE_COLOR) && !tileInitialize )
  {   
    // Check to see if all pixels in this tile are same. If so apply single color compression, 
    // otherwise no compression is applied
    if ( !IsTileAllSamePixel(entireTile, (short) TILE_WIDTH,(short) TILE_WIDTH, &singleColorPixel) ) {
      compression = NONE; 
      idCodec   = ConvertCompressionOption();
    }
    else
    {
      // Only rotate color components if opacity is not last in order   PTCH_202
      if (!((base == SPACE_32_BITS_RGBA)                //  PTCH_202
        ||  (base == SPACE_32_BITS_YCCA))) {              //  PTCH_202
        // Shift Pixels so that compsubtype gets written
        // as RGBA not ARGB
        // This should actually be the same channel order
        // as specified in Subimage color of Image Contents
        Pixel tmpPixel;
  
        tmpPixel.alpha = singleColorPixel.rouge;
        tmpPixel.rouge = singleColorPixel.vert;
        tmpPixel.vert  = singleColorPixel.bleu;
    
        if( nbChannels > 3)
          tmpPixel.bleu = singleColorPixel.alpha;
        else
          tmpPixel.bleu = 0x00;
  
        singleColorPixel = tmpPixel;
      }                               //  PTCH_202
    }
  }

  // TEMPORARY IN THIS VERSION: when no compression, should compress 32->nbChannel

  if ((idCodec == TLC_Aucun) && (!fatherSubImage->fatherFile->existAlphaChannel))
    idCodec = TLC_32Vers24;
  
  compressionSubtype = 0; // subtype is 0 for all cases except TLC_SingleColor.

  switch(idCodec) {
    case TLC_Aucun: // If no Compression
      // Set the tilesize
      sizeCompressed = TILE_WIDTH * TILE_WIDTH * sizeof(Pixel);

      // Set the output buffer to write
      buffer = (char *)(entireTile);
      break; 
    
    case TLC_SingleColor: // Single color compression
      // Set the tile values for a single color tile (see spec p.46-48)
      posPixelFic     = 0;
      tileSize      = 0;
      compression     = SINGLE_COLOR;
      compressionSubtype  = (long)(singleColorPixel);
#ifndef IN_LITTLE_ENDIAN
    compressionSubtype = (long) SwapBytes( (int32)compressionSubtype );
#endif
      // Update local management values
      sizeCompressed    = 0;
      buffer        = NULL;
      break;   
    
    case TLC_JPEG: // JPEG compression
      // Set compression subtype ( The subtype is only used by jpeg compression )
      compressionSubtype  = ((PResolutionFlashPix *)fatherSubImage)->compressionSubtype;

      // If channel is less than 4, compression the data 
      if ( nbChannels < 4 ) {
        // Get the TLC_32Vers24 compressor
        monCompresseur = (*tousLesCodecs)[TLC_32Vers24];
        monCompresseur->Lock();       // Lock buffers used by the compressor

        // In the case of simple compression, we have to set the number of channels and the direction
        // of the compression. This is optimized only for the most common cases.
        Boolean leftShift = false;
        if ((base == SPACE_32_BITS_RGBA) || (base == SPACE_32_BITS_YCCA)) {
          leftShift   = true;
        }
        ((obj_Compresseur32Vers24 *)monCompresseur)->SetCompressionParameters(nbChannels,leftShift);
          
        if(!monCompresseur->Compresse((Ptr)entireTile,(short) TILE_WIDTH, 
            (short)TILE_WIDTH, &buffer, &tileSize)) {
          fileFPX->CompressionError();  // Compression failed => Signal error
          posPixelFic = -1;       // Compression failed => Delete reference pointer
          monCompresseur->UnLock();   // Unlock buffers used by the compressor
          assert (false);

          if ((width < TILE_WIDTH)  || (height < TILE_WIDTH) || (used != base))
            FastDeleteArray( entireTile, Pixel);
          status = FPX_MEMORY_ALLOCATION_FAILED;  // CHG_FILE_ERR - the only error that occurs
          goto RETURN;
        }

        LockDecompress ();          
        AllocDecompress(tileSize);
        if(decompressBuffer == NULL) {        // CHG_FILE_ERR - check for allocation failure
          status = FPX_MEMORY_ALLOCATION_FAILED;
          goto RETURN;
        }
        memcpy(decompressBuffer, buffer, tileSize);
      } else
        decompressBuffer = (char *)entireTile;
      
      // Get the jpeg compressor
      monCompresseur = (*tousLesCodecs)[idCodec];
      
      unsigned char internalConv;
      
      if(base == SPACE_32_BITS_RGBA || base == SPACE_32_BITS_RGB || 
         base == SPACE_32_BITS_ARGB) {
        internalConv = 1;
      }
      else {
        internalConv = 0;
        // clear the internal rotation bits from compression subtype
        compressionSubtype &= 0xFF00FFFF; // Clear chroma subsample flags
      }
        
        
      // Set the jpeg parameters
      if ( status = (FPXStatus)((PCompressorJPEG *)monCompresseur)->SetCompressionParameters( 
                      GET_InterleaveType(compressionSubtype),
                      GET_ChromaSubSample(compressionSubtype),
                      // GET_InternalColorConv(compressionSubtype),
                      internalConv, 
                      compressionSubtype,
                      qualityFactor,
                      nbChannels,
                      NULL,
                      0,
                      FALSE )) {
        fileFPX->CompressionError();  // Compression failed => Signal error
        posPixelFic = -1;       // Compression failed => Delete reference pointer
        assert (false);

        if ((width < TILE_WIDTH)  || (height < TILE_WIDTH) || (used != base))
          FastDeleteArray(entireTile,Pixel);
        status = (FPXStatus)jpegErrorToFPXerror( status);
        goto RETURN;
      }
      
      // Do the JPEG compression

      if ( ((PCompressorJPEG *)monCompresseur)->Compress((unsigned char *)decompressBuffer, 
            (short)TILE_WIDTH, (short)TILE_WIDTH, (unsigned char **)&buffer, 
            &sizeCompressed))  {
        fileFPX->CompressionError();  // Compression failed => Signal error
        posPixelFic = -1;       // Compression failed => Delete reference pointer
        assert (false);

        if ((width < TILE_WIDTH)  || (height < TILE_WIDTH) || (used != base))
          FastDeleteArray(entireTile,Pixel);
        status = FPX_MEMORY_ALLOCATION_FAILED;  // CHG_FILE_ERR - this is the only Compresse error
        goto RETURN;
      }
      if ( nbChannels < 4 )
        UnLockDecompress ();        // Unflag decompress buffer

      // Write jpeg header to the image content property set
      WriteHeader(fileFPX, ((PCompressorJPEG *)monCompresseur)->GetHeaderBuf(),
          ((PCompressorJPEG *)monCompresseur)->GetHeaderSize());  
      break;
       
    default:
      // Get the corresponding compressor
      monCompresseur = (*tousLesCodecs)[idCodec];
      monCompresseur->Lock();         // Lock buffers used by the compressor
      
      if (idCodec == TLC_32Vers24) {
        // In the case of simple compression, we have to set the number of channels and the direction
        // of the compression. This is optimized only for the most common cases.
        long nbChan = ((PResolutionFlashPix*)fatherSubImage)->nbChannels;
        Boolean leftShift = false;
        if ((base == SPACE_32_BITS_RGBA) || (base == SPACE_32_BITS_YCCA)) {
          leftShift   = true;
        }
        ((obj_Compresseur32Vers24 *)monCompresseur)->SetCompressionParameters(nbChan,leftShift);
      }
      
      if (!monCompresseur->Compresse((Ptr)entireTile,(short) TILE_WIDTH, 
        (short) TILE_WIDTH, &buffer, &sizeCompressed)) {
        fileFPX->CompressionError();    // Compression failed => Signal error
        posPixelFic = -1;         // Compression failed => Delete reference pointer
        monCompresseur->UnLock();     // Unlock buffers used by the compressor
        assert (false);

        if ((width < TILE_WIDTH)  || (height < TILE_WIDTH) || (used != base))
          FastDeleteArray(entireTile,Pixel);
        status = FPX_MEMORY_ALLOCATION_FAILED;  // CHG_FILE_ERR - this is the only Compresse error
        goto RETURN;
      }
      break;
  }
    
  // Compute position of the tile in file
  // CAUTION : go to the end of the file if the new tile is bigger than the old one
  // NO GARBAGE COLLECTION WITHIN THE FILE
  if ((posPixelFic < 0) || (tileSize < sizeCompressed)) {
    if (subStreamData->GetEndOfFile( &posPixelFic) == 0)
      status = FPX_FILE_READ_ERROR;
    posPixelFic;
  }

  if ((status == FPX_OK) && buffer) {
    tileSize = sizeCompressed;          // Set tilesize as size of compressed data
    
    if (subStreamData->Seek(posPixelFic) == 0)  // Go to the right position in the stream
      status = FPX_FILE_READ_ERROR;
    else                    // Write the pixels or the compressed tile
      if (subStreamData->Write((unsigned char *)buffer,tileSize) == 0)
        status = subStreamData->getFPXStatus();
  }

  // Clean up tile and release memory
  if ((idCodec != TLC_Aucun) && (idCodec != TLC_SingleColor)) { // If Compression
    ptr_Compresseur monCompresseur = (*tousLesCodecs)[idCodec];
    monCompresseur->UnLock();                 // Unlock buffers used by the compressor
  }

#ifdef Memoire_Debug
  VISU2 "Writing this tile successfull\n" FIN
#endif

 RETURN: 
  if (wasLocked == false)
    UnLock();
    
  if ((width < TILE_WIDTH)  || (height < TILE_WIDTH) || (used != base))
    FastDeleteArray(entireTile,Pixel);

  if (status == FPX_OK) {
    // Update the global modification flag
    PFileFlashPixIO* father = (PFileFlashPixIO*)(fatherSubImage->fatherFile);
    father->tilesHasBeenModified = TRUE;
  }
  return (status);
}


//  ----------------------------------------------------------------------------
// Read the data from the file directly in the passed parameters
// This returns the raw, possibly compressed tile data as stored in the file
// Returns: FPX_MEMORY_ALLOCATION_FAILED
//      FPX_ERROR           - if tile is missing in the file
//      FPX_FILE_READ_ERROR
//
FPXStatus PTileFlashPix::ReadRawTile (FPXCompressionOption*   compressOption, 
                 unsigned char*     compressQuality,
                 long*          compressSubtype,
                 unsigned long*     dataLength,
                 void**         data)
{
  // Declare some management variable

  PFlashPixFile *fileFPX = NULL;
  Boolean     wasLocked = IsLocked();
  FPXStatus   status = FPX_OK;


  // Init the passed parameters with default values
  *compressOption   = NONE;
  *compressQuality  = 0;
  *compressSubtype  = 0;
  *dataLength     = 0;
  *data         = NULL;

  // If the tile has never been written, return an error
  if (posPixelFic < 0) {
    status = FPX_ERROR;
    goto exit;
  }

  // Test if the file is already open
  if (fatherSubImage->fatherFile->filePtr)
    fileFPX = (PFlashPixFile*)fatherSubImage->fatherFile->filePtr;

  else {
    // This should never happen, however, we try to open the file
    assert(false);    // beeps...

    // Open the FPX file
    PFileFlashPixIO* parentFile = (PFileFlashPixIO*)(fatherSubImage->fatherFile);

    // These calls are flacky because the pointed values may be obsolete by now... may crash...
    if (parentFile->owningStorage)
      fileFPX = new PFlashPixFile (parentFile->owningStorage,parentFile->storageName,mode_Lecture);
    else
      fileFPX = new PFlashPixFile (parentFile->fileName,parentFile->storageName,mode_Lecture);

    // If error while opening : signal error and exit with error
    if (fileFPX->Erreur() != FPX_OK) {
      parentFile->UpdateErrorCount();
      fileFPX->SignaleErreur();
      status = FPX_FILE_READ_ERROR;
      goto exit;
    }
  }

  // dk 6aug96.  need block so subStreamData goes out of scope before exit
  {
      OLEHeaderStream *subStreamData = ((PResolutionFlashPix*)fatherSubImage)->GetSubStreamData();
  
      if (IVUE_System->GetErrorsList() != NULL) {
      status = FPX_FILE_READ_ERROR;
      goto exit;
      }

      Lock();

      // Allocate the required memory
      char* buffer;

      FastAllocArray(buffer, char, tileSize);
      if (buffer == NULL) {
      status = FPX_MEMORY_ALLOCATION_FAILED;
      goto exit;
      }
  
      // Read pixels data (at last...)
      if (subStreamData->Seek(posPixelFic) == false) {
      FastDeleteArray(buffer, char);
      status = FPX_FILE_READ_ERROR;
      goto exit;
      }

      if (subStreamData->Read(buffer,tileSize) == 0) {
      FastDeleteArray(buffer, char);
      status = FPX_FILE_READ_ERROR;
      goto exit;
      }

      // Update the other parameters
      *data      = buffer;
      *dataLength      = tileSize;
      *compressOption  = (FPXCompressionOption)(compression);
      *compressQuality = qualityFactor;
      *compressSubtype = compressionSubtype;
  }

exit: 
  // Close the file if IVUE opened in read only
  if (!fatherSubImage->fatherFile->filePtr)
    delete fileFPX;
  if (wasLocked == false)
    UnLock();
  return status;
}


// ---------------------------------------------------------------------------------
// Read a tile's raw data and decompress it. The data is placed in 'rawPixels'.
// If 'rawPixels' contains data, then the data is lost.
//
// Returns: FPX_MEMORY_ALLOCATION_FAILED
//      FPX_FILE_READ_ERROR
//      FPX_INVALID_COMPRESSION_ERROR - if no decompressor available
//
FPXStatus PTileFlashPix::ReadRawPixels()
{
  ptr_Compresseur monDecompresseur; 
  PFlashPixFile *fileFPX;
  Pixel       *entireTile;
  register long   TILE_WIDTH = fatherSubImage->fatherFile->tileWidth;
  Boolean     wasLocked = false;
  FPXStatus   status = FPX_OK;
  OLEHeaderStream *subStreamData = NULL;
  
  FPXBaselineColorSpace base = ((PResolutionFlashPix*)fatherSubImage)->baseSpace;
  FPXBaselineColorSpace used = ((PFileFlashPixIO*)fatherSubImage->fatherFile)->usedSpace;

  // Allocate pixel buffer, if it's hasn't been done yet.
  if (rawPixels == NULL)
    if (AllocateRawPixels())
      return FPX_MEMORY_ALLOCATION_FAILED;
  rawPixelsSpace = base;

  // If the tile has never been written (i.e., it's not in the file), then fill it with the 
  //  background color and return. The file should always have all the tiles, unless the
  //  file is in the process of creation.
  if ((posPixelFic < 0) && (idCodec != TLC_SingleColor)) {  // PTCH_NEGONE
    // If there was an error, fill the tile with the background color.
    Pixel  BACKGROUND = fatherSubImage->fatherFile->backgroundUsed;
    Pixel* pt = rawPixels;
    long i = width * height;
    
    while (i--) 
      *pt++ = BACKGROUND;
    return FPX_OK;
  }
  
  // Test that the file is already open
  if (fatherSubImage->fatherFile->filePtr)
    fileFPX = (PFlashPixFile*)fatherSubImage->fatherFile->filePtr;
  else {
    // This should never happen, however, we try to open the file
    assert(false);        // beeps...

    // Open the FPX file
    PFileFlashPixIO* parentFile = (PFileFlashPixIO*)(fatherSubImage->fatherFile);

    // These calls are flacky because the pointed values may be obsolete by now... may crash...
    if (parentFile->owningStorage)
      fileFPX = new PFlashPixFile (parentFile->owningStorage,parentFile->storageName,mode_Lecture);
    else
      fileFPX = new PFlashPixFile (parentFile->fileName,parentFile->storageName,mode_Lecture);

    // If error while opening : signal error and exit with error
    if (fileFPX->Erreur() != FPX_OK) {
      parentFile->UpdateErrorCount();
      fileFPX->SignaleErreur();
      freshPixels = 0;
      status = FPX_FILE_READ_ERROR;
      goto RETURN;
    }
  }

  subStreamData = ((PResolutionFlashPix*)fatherSubImage)->GetSubStreamData();
  if (IVUE_System->GetErrorsList() != NULL) {
    status = FPX_FILE_READ_ERROR;
    goto RETURN;
  }
  wasLocked = IsLocked();
  Lock();

  // This is a left or bottom edge tile and is not full size, then allocate a temporary
  //  buffer of full tile size.
  if ((width < TILE_WIDTH)  || (height < TILE_WIDTH)) {
    FastCallocArray( entireTile, Pixel, TILE_WIDTH * TILE_WIDTH);
    if (entireTile == NULL) {
      status = FPX_MEMORY_ALLOCATION_FAILED;
      goto RETURN;
    }
  } else
    entireTile = rawPixels;
  
  // Read the data from the file and decompress it into 'entireTile'
  switch(idCodec) {
    case TLC_Aucun:       // If no Compression
      // Read pixels data (at last...)
      if (subStreamData->Seek(posPixelFic) == false)
        status = subStreamData->getFPXStatus();
      else if (subStreamData->Read((unsigned char*)entireTile,tileSize) == 0)
        status = subStreamData->getFPXStatus();
      break; 

    case TLC_SingleColor: {   // Fill the tile with the single color info
      Pixel tmpPixel;
      Pixel *pt = entireTile;
      long  nbPixels = TILE_WIDTH*TILE_WIDTH;
      
#ifndef IN_LITTLE_ENDIAN
    compressionSubtype = (long) SwapBytes( (int32)compressionSubtype );
#endif
      unsigned char *tmpptr = (unsigned char*)&compressionSubtype;
      
      long nbChan = ((PResolutionFlashPix*)fatherSubImage)->nbChannels;

      if(nbChan == 4) {
        tmpPixel.alpha = tmpptr[0];
        tmpPixel.rouge = tmpptr[1];
        tmpPixel.vert  = tmpptr[2];
        tmpPixel.bleu  = tmpptr[3];
      }
      else {    
        tmpPixel.alpha = tmpptr[3];
        tmpPixel.rouge = tmpptr[0];
        tmpPixel.vert  = tmpptr[1];
        tmpPixel.bleu  = tmpptr[2];
      }
      while (nbPixels--)
        *pt++ = tmpPixel;
#ifndef IN_LITTLE_ENDIAN
    compressionSubtype = (long) SwapBytes( (int32)compressionSubtype );
#endif
      break; }

    
    case TLC_JPEG: {        // JPEG decompression
      unsigned char*  jpegHeader = ((PResolutionFlashPix*)fatherSubImage)->jpegHeader; 
      unsigned long   headerSize = ((PResolutionFlashPix*)fatherSubImage)->headerSize; 

      decompressorIsMissing = true;       // look for the decompressor

      // Get the corresponding compressor
      monDecompresseur = (*tousLesCodecs)[idCodec];

      if (monDecompresseur != NULL)
        // Read jpeg header to the image content property set
        if ( ReadHeader(fileFPX, &jpegHeader, &headerSize) ) {
          ((PResolutionFlashPix*)fatherSubImage)->jpegHeader = jpegHeader; 
          ((PResolutionFlashPix*)fatherSubImage)->headerSize = headerSize;                                    
          decompressorIsMissing = false;             
        }
      if (!decompressorIsMissing) {     // If decompressor is present
        LockDecompress ();          // Flag decompress buffer in use
        AllocDecompress(tileSize);      // Allocate a buffer for compressed data
        
        if (!decompressBuffer)        // If no buffer allocatedÎ
          decompressorIsMissing = true; // Îdecompression failed
        else {
          // Read pixels data (at last...)
          if (subStreamData->Seek( posPixelFic) == false) {
            status = FPX_FILE_READ_ERROR;
            goto RETURN;
          }
          if (subStreamData->Read( (unsigned char*)decompressBuffer, tileSize) == 0) {
            status = FPX_FILE_READ_ERROR;
            goto RETURN;
          }
          // Set the jpeg parameters
          if ( status = (FPXStatus)((PCompressorJPEG *)monDecompresseur)->SetCompressionParameters( 
                  GET_InterleaveType(compressionSubtype),
                  GET_ChromaSubSample(compressionSubtype),
                  GET_InternalColorConv(compressionSubtype),
                  compressionSubtype, 
                  qualityFactor,
                  nbChannels,
                  jpegHeader,                 
                  headerSize,
                  TRUE )) { 
            status = (FPXStatus)jpegErrorToFPXerror( status);
            goto RETURN;
          }

          // Do the JPEG decompression
          if ( status = (FPXStatus)((PCompressorJPEG *)monDecompresseur)->Decompress((unsigned char *)entireTile, 
              (short) TILE_WIDTH,(short) TILE_WIDTH, (unsigned char*)decompressBuffer, tileSize) ) {
            status = (FPXStatus)jpegErrorToFPXerror( status);
            goto RETURN;
          }

          // Convert to 4-channel colorspace
          if ( nbChannels < 4 ) {
            // Get a pointer to the simple expansion function
            monDecompresseur = (*tousLesCodecs)[TLC_32Vers24];

            // Determine the alignment direction
            Boolean leftShift = false;

            if (((base == SPACE_32_BITS_RGB) && (used == SPACE_32_BITS_RGBA)) || 
              ((base == SPACE_32_BITS_YCC) && (used == SPACE_32_BITS_YCCA))) {
              leftShift   = true;
              rawPixelsSpace = used;
            }

            // Set the expansion parameters
            ((obj_Compresseur32Vers24 *)monDecompresseur)->SetCompressionParameters(nbChannels,leftShift);

            LockDecompress ();          // Flag decompress buffer in use
            AllocDecompress(TILE_WIDTH * TILE_WIDTH * nbChannels);      // Allocate a buffer for compressed data
            if(decompressBuffer == NULL) {
              status = FPX_MEMORY_ALLOCATION_FAILED;
              goto RETURN;
            }
            // Expand the buffer
            memcpy( decompressBuffer, entireTile, TILE_WIDTH * TILE_WIDTH * nbChannels);
            monDecompresseur->Decompresse((Ptr)entireTile,(short) TILE_WIDTH,
                  (short) TILE_WIDTH, decompressBuffer, 
                  TILE_WIDTH * TILE_WIDTH * nbChannels);
            UnLockDecompress ();        // Unflag decompress buffer
          }
        }
        UnLockDecompress ();            // Unflag decompress buffer
      }
      break;
    }
    default:
    {
      decompressorIsMissing = true;         // look for the decompressor

      // Check if the tile is valid 
      if ( compression == 0xFFFFFFFF ) 
        break;

      monDecompresseur = (*tousLesCodecs)[idCodec];

      if (monDecompresseur != NULL)
        if ( monDecompresseur->DecompresseurPresent() )
          decompressorIsMissing = false;

      if (!decompressorIsMissing) {     // If decompressor is present
        if (idCodec == TLC_32Vers24) {

          // In the case of simple expansion, we have to set the number of channels and the direction
          // of the expansion. This is optimized only for the most common cases.
          long  nbChan = ((PResolutionFlashPix*)fatherSubImage)->nbChannels;
          Boolean leftShift = false;

          if (((base == SPACE_32_BITS_RGB) && (used == SPACE_32_BITS_RGBA)) || 
            ((base == SPACE_32_BITS_YCC) && (used == SPACE_32_BITS_YCCA))) {
            leftShift   = true;
            rawPixelsSpace = used;
          }
          ((obj_Compresseur32Vers24 *)monDecompresseur)->SetCompressionParameters(nbChan,leftShift);
        }

        LockDecompress ();          // Flag decompress buffer in use
        AllocDecompress(tileSize);      // Allocate a buffer for compressed data

        if (!decompressBuffer)        // If no buffer allocatedÎ
          decompressorIsMissing = true; // Îdecompression failed
        else {
          // Read pixels data (at last...)
          if (subStreamData->Seek(posPixelFic) == false) {
            status = FPX_FILE_READ_ERROR; // CHG_FILE_ERR - better error report
            goto RETURN;
          }
          if (subStreamData->Read((unsigned char*)decompressBuffer,tileSize) == 0) {
            status = FPX_FILE_READ_ERROR; // CHG_FILE_ERR - better error report
            goto RETURN;
          }
          if ( !monDecompresseur->Decompresse((Ptr)entireTile,(short) TILE_WIDTH,
                (short) TILE_WIDTH, decompressBuffer, tileSize) )
            decompressorIsMissing = true; // Decompression failed
        }
        UnLockDecompress ();        // Unflag decompress buffer
      }
      break;
    }
  }     

  // If the tile is not full-sized, then copy the pertinent pixels into 'rawPixels'
  if ((width < TILE_WIDTH)  || (height < TILE_WIDTH)) {
    short i;
    Pixel *source = entireTile;
    Pixel *dest = rawPixels;
  
    for (i = 0; i < height; i++, source += TILE_WIDTH, dest += width)
      memcpy (dest, source, width * sizeof(Pixel));
    FastDeleteArray( entireTile, Pixel);
  }

  // Invert alpha channel if requested by user
  if (fatherSubImage->fatherFile->inverseAlphaChannel && invertLUT) {
    unsigned char* pt = (unsigned char*)(rawPixels);
    long i, j;

    for (i = 0; i < height; i++)
      for (j = 0; j < width; j++, pt+=4)
        *pt = invertLUT[*pt];
  }

 RETURN:
  freshPixels = 0;                // No modified pixels in buffer yet
  pixelsStale = true;               // Buffer has not been modified yet
  
  if (wasLocked == false)
    UnLock();
  
  if (status != FPX_OK) {
    if (decompressorIsMissing)
      fileFPX->CompressionError();        // Signal error
      
    int32 *pt = (int32*)(rawPixels);
    int32 i, j, pat_i, pat_j;

    // CHG_FILE_ERR - fixed the following loops so only height x width pixels are
    //        written into the rawPixels buffer (not TILE_WIDTH x TILE_WIDTH)
    for (i = 0, pat_i = 0; i < height; i++, pat_i++) {
      if (pat_i == 8)
        pat_i = 0;
      for (j = 0, pat_j = 8 - pat_i; j < width; j++, pat_j++, pt++) {
        if (pat_j == 8) pat_j = 0;
        switch (pat_j) {
          case 0 :
          case 4 :
            *pt = 0x7F7F7F; break;
          case 1 :
          case 2 :
          case 3 :
            *pt = 0xFFFFFF; break;
          case 5 :
          case 6 :
          case 7 :
            *pt = 0; break;
        }
      }
    }
  }
  
  // Close the file if IVUE opened in read only
  if (!fatherSubImage->fatherFile->filePtr)
    delete fileFPX;
    
  return status;
}


// ---------------------------------------------------------------------------------
// Return a tile's display-modified data. If necessary, this will read the raw data in 
//  from the file. If the 'rawPixels' have already been read and decompressed, then 
//  file reading is not necessary.
// The data returned has contrast, filtering and colortwist applied to it. This data
//  is cached in 'pixels' and, if missing, is created during this call.
//
// Returns: FPX_MEMORY_ALLOCATION_FAILED
//      FPX_COLOR_CONVERSION_ERROR    - due to filtering problems
//      FPX_INVALID_COMPRESSION_ERROR - if no decompressor available
//
FPXStatus PTileFlashPix::Read()
{
  ptr_Compresseur monDecompresseur; 
  ViewImage     *imageParam = fatherSubImage->fatherFile->imageParam;
  Boolean     wasLocked = IsLocked();
  FPXStatus   status = FPX_OK;
  
  // Check if the pixels are already cached, or if they have been modified
  FPXBaselineColorSpace base = ((PResolutionFlashPix*)fatherSubImage)->baseSpace;
  FPXBaselineColorSpace used = ((PFileFlashPixIO*)fatherSubImage->fatherFile)->usedSpace;

  
  // Determine if we need to read new 'rawPixels' or create a new set of 'pixels'
  Boolean   needNewPixels = false;

  if (pixels == NULL)           // If the 'pixels' buffer has not been allocated, 
    needNewPixels = true;       //  then a new set of pixels needs to be made
  else if (pixelsStale)         // Otherwise, if pixels contains old image data
    needNewPixels = true;       //  then a new set of pixels needs to be made
    
  if (imageParam) {           // If there are display-modifications for image
    if (imageParam->GetDirtyCount()   //  and the # times modifications have been set 
          != dirtyCount)      //  does not match the count when pixels were created
      needNewPixels = true;     //  then a new set of pixels needs to be made
  }
  if (needNewPixels == true)        // If a new set of 'pixels' needs to be made
    if (rawPixels == NULL)        //  and we don't have any raw pixels cached
      if (status = ReadRawPixels()) //  then read-in it's  uncompressed data
        return status;
                      // If there are no viewing/display modifications 
  if (needNewPixels == false) {     //  and we don't need to update pixels
    TouchRawPixelsBuffer( );      //  then we can use 'rawPixels'
    return FPX_OK;
  }

  // A new set of 'pixels' needs to be made. Make sure there is memory available
  Lock();
  if (pixels == NULL)
    if (AllocatePixels()) {
      if (wasLocked == false)
        UnLock();
      return FPX_MEMORY_ALLOCATION_FAILED;
    }
  dirtyCount = fatherSubImage->fatherFile->imageParam->GetDirtyCount();
  viewingParamApplied = fatherSubImage->fatherFile->applyParam;

  // Copy rawPixels into pixels. Set it's timestamp.
  memcpy (pixels, rawPixels, width * height * sizeof(Pixel));
  pixelsSpace = rawPixelsSpace;
  TouchPixelsBuffer( );   

  // Take the viewing display parameters into account 
  if(imageParam && viewingParamApplied) {

    if (imageParam->HasFilteringValue())      // Apply filtering (sharpen/blur)
      if ((status = ApplyFilter( rawPixelsSpace)) != FPX_OK)
        return status;

    if (imageParam->HasColorTwist()) {        // Apply colortwist to 'pixels'
      PColorTwist colorTwist;

      imageParam->GetColorTwist( &colorTwist);

      switch (pixelsSpace) {
        case  SPACE_32_BITS_RGB:
        case  SPACE_32_BITS_ARGB:
        case  SPACE_32_BITS_RGBA: {
          PColorTwist RGBtorgb (RGB8_to_rgb);
          PColorTwist rgbtoycc (rgb_to_ycc);
          PColorTwist ycctorgb (ycc_to_rgb);
          PColorTwist rgbtoRGB (rgb_to_RGB8);
    
          colorTwist = rgbtoRGB * ycctorgb * colorTwist * rgbtoycc * RGBtorgb;
          break;  }
        case  SPACE_32_BITS_YCC:
        case  SPACE_32_BITS_M:
        case  SPACE_32_BITS_MA: {
          PColorTwist YCC8toycc (YCC8_to_ycc);
          PColorTwist ycctoYCC8 (ycc_to_YCC8);
    
          colorTwist =  ycctoYCC8 * colorTwist * YCC8toycc;
          
          // CHG_MONO_TINT - FlashPix spec states that monochrome images are tinted using
          //  the colorTwist matrix. To preserve color information, if the 'used' space
          //  (i.e., the space used by the calling applic) has color, then we will set the
          //  'pixelSpace' (the tile buffer after color transforms) to YCC. The matrix we
          //  just created will give YCC output. There is a final colorspace conversion
          //  at the end of this section that will do RGB->YCC conversion if necessary.
          if ((used != SPACE_32_BITS_M) && (used != SPACE_32_BITS_MA))
            if (pixelsSpace == SPACE_32_BITS_M)
              pixelsSpace = SPACE_32_BITS_YCC;
            else if (pixelsSpace == SPACE_32_BITS_MA)
              pixelsSpace = SPACE_32_BITS_YCCA;
          break; }
        case  SPACE_32_BITS_YCCA:
        case  SPACE_32_BITS_AYCC: {
          PColorTwist YCC8toycc (YCC8_to_ycc);
          PColorTwist ycctoYCC8 (ycc_to_YCC8);
  
          colorTwist =  ycctoYCC8 * colorTwist * YCC8toycc;
          colorTwist.UseAlphaChannel();
          break; }
      }
      colorTwist.ApplyToPixelBuffer( pixels, pixelsSpace, width * height);
    }
    
    if (imageParam->HasContrastValue()) {         // Apply contrast to 'pixels'
      float   contrastValue;
    
      // Since contrast is applied in RGB space, if the pixels are currently in a YCC space
      //  but are to be returned in RGB or monochrome space, then apply that conversion
      //  now.
      if ((pixelsSpace == SPACE_32_BITS_YCC)
      || (pixelsSpace == SPACE_32_BITS_YCCA)
      || (pixelsSpace == SPACE_32_BITS_AYCC))
        if (!((used == SPACE_32_BITS_YCC) 
          ||  (used == SPACE_32_BITS_YCCA)
          ||  (used == SPACE_32_BITS_AYCC))) {
            // If need be, preserve the opacity component for clippint the
            //  contrast-adjusted component(s)
            FPXBaselineColorSpace tmpPixelSpace = used;
            
          if ((pixelsSpace == SPACE_32_BITS_YCCA)
          || (pixelsSpace == SPACE_32_BITS_AYCC))
            if (used == SPACE_32_BITS_RGB)
              tmpPixelSpace = SPACE_32_BITS_ARGB;
        
          ConvertPixelBuffer((unsigned char*)(pixels), width * height, 
                  pixelsSpace, tmpPixelSpace);
          pixelsSpace = tmpPixelSpace;
        }
        imageParam->GetContrast( &contrastValue);
        Contrast( contrastValue, pixelsSpace, pixels, width * height);
      }
  } 

  // Do the color conversion
  if (pixelsSpace != used) {
    ConvertPixelBuffer((unsigned char*)(pixels), width * height,pixelsSpace, used);
    pixelsSpace = used;
  }

  if (wasLocked == false)
    UnLock();
  pixelsStale = false;
  

// VISU2 "Read tile %d of the image %s\n", identifier, fatherSubImage->fatherFile->fileName.nom.Texte() FIN

  return (FPX_OK);
}



// --------------------------------------------------------------------------
// This method makes a version of the raw pixels which is padded on all four
//  sides by 'padLen'. If there are neighboring tiles, then the pad data is
//  obtained from them. If this is an edge tile, then the padding is made by
//  pixel replication (as per the FlashPix specification).
// NOTE: This code assumes that 'padLen' is never greater than the width of a
//  full tile (which is currently 64x64 Pixels). RIght & bottom edge conditions
//  are handled properly for neighboring tiles which are not full-size.
//
FPXStatus PTileFlashPix::makePaddedRawPixels (long padLen, Pixel **paddedRawPixels)
{
  long      x, y;
  Pixel     *srcP, *dstP;
  long      srcStride,          // Source Pixel pointer increment
          padWidth,         // Width (in Pixels) of padded tile being filled
          srcCnt,           // # of rows or cols source tile can contribute
          numTiles,         // Number of tiles in this resolution
          neighborID;         // (zero-based) identifier of a tile
  PTileFlashPix *tileArr,         // Tile list for this resolution
          *srcTile;         // Neighboring tile for padding source
  Boolean     wasLocked;
  FPXStatus   status = FPX_OK;
  
  wasLocked = IsLocked();           // Remember if this tile was initially locked
  Lock();                   // Lock this tile so it's 'rawPixels' memory
                        //  does not get re-allocated
  
  FastAllocArray( *paddedRawPixels, Pixel, (width + (2 * padLen)) * (height + (2 * padLen)));
  if (*paddedRawPixels == NULL) {
    status = FPX_MEMORY_ALLOCATION_FAILED;
    goto RETURN;
  }
  
  padWidth = width + (2L * padLen);
  tileArr  = (PTileFlashPix*)fatherSubImage->tiles;
  numTiles = fatherSubImage->nbTilesW * fatherSubImage->nbTilesH;


  // Surrounding tile positions for notes:  |0|1|2|
  //                      |3|x|4|
  //                      |5|6|7|
  //  where 'x' is the position of the tile to be padded. 

  // ------------------  Do top pad fill -- Position [1]
  neighborID = identifier - fatherSubImage->nbTilesW;
  if( neighborID >= 0) {            // If there is a neighboring tile above

    srcTile = tileArr + neighborID;     // Get it's address
    if( srcTile == NULL)          // If the tile doesn't exist
      return FPX_ERROR;         //  then we're in big trouble
    srcTile->TouchRawPixelsBuffer( );   // Set access time stamp
    srcP = srcTile->rawPixels;        // Get the address of it's raw pixels

    if( srcP == NULL) {           // If there aren't any raw pixels in memory
      if( (status = srcTile->ReadRawPixels()) != FPX_OK)  // Read-in neighbor tile's raw pixels
        return status;
      srcP = srcTile->rawPixels;
    }
    // Calc where the desired data is located in the source tile and set the read increment
    //  to be a full line of pixels
    srcP += ((srcTile->height - padLen) * srcTile->width);
    srcStride = srcTile->width;
  }
  else {                    // Else there is no neighbor tile above,
    srcP = rawPixels;           //  so just point to the top line of the target tile
    srcStride = 0;              //  and set increment to 0 se we keep copying the
  }                     //  same line over and over to form padding

  dstP = *paddedRawPixels + padLen;     // Skip over left pad area of destination buffer
  for( y = 0; y < padLen; y++) {        //  and copy data into it
    memcpy( dstP, srcP, width * sizeof(Pixel) );
    srcP += srcStride;
    dstP += padWidth;
  }

  // ------------------  Copy source into center of padded dest -- Position [x]
  srcP = rawPixels;             // Copy from the stored raw pixels 
  srcStride = width;
  dstP = *paddedRawPixels + (padWidth * padLen) + padLen;
  for( y = 0; y < height; y++) {
    memcpy( dstP, srcP, width * sizeof(Pixel) );
    srcP += srcStride;
    dstP += padWidth;
  }

  // ------------------  Do bottom pad fill -- Position [6]
  dstP = *paddedRawPixels + ((height + padLen) * padWidth) + padLen;
  neighborID = identifier + fatherSubImage->nbTilesW;

  if( neighborID < numTiles) {        // If there is a neighboring tile below

    srcTile = tileArr + neighborID;     // Get it's address
    if( srcTile == NULL)          // If the tile doesn't exist
      return FPX_ERROR;         //  then we're in big trouble
    srcTile->TouchRawPixelsBuffer( );   // Set access time stamp
    srcP = srcTile->rawPixels;        // Get the address of it's raw pixels

    if( srcP == NULL) {           // If there aren't any raw pixels in memory
      if( (status = srcTile->ReadRawPixels()) != FPX_OK)  // Read-in neighbor tile's raw pixels
        return status;
      srcP = srcTile->rawPixels;
    }
    srcStride = srcTile->width;
    srcCnt = srcTile->height;
  }
  else {                    // No tile below. Replicate last line of tile.
    srcP = dstP - padWidth;         // Back one line's length from destination Pixel
    srcStride = 0;              // Never really used for this case
    srcCnt = 0;               // Set to 0 so that srcP does not get incremented
  }   

  for( y = 0; y < padLen; y++) {
    memcpy( dstP, srcP, width * sizeof(Pixel) );
    dstP += padWidth;
    srcCnt--;
    if( srcCnt > 0)
      srcP += srcStride;
  }
  

  // ------------------  Start left fills
  if( (identifier % fatherSubImage->nbTilesW) == 0) {
    // -------------  Do left fills -- Positions [0], [3] and [5]
    // The tile being padded is at the left edge, so all we have to do is to copy/extend 
    //  the leftmost pixels that have already been loaded into the padded buffer to fill
    //  the pad area along the left side.
    dstP = *paddedRawPixels;              // 1st pixel in buffer 
    for (y = 0; y < height + (2L * padLen); y++) {
      srcP = dstP + padLen;             // 1st pixel in line copied from src
      for (x = 0; x < padLen; x++)
        *dstP++ = *srcP;
      dstP += (padWidth - padLen);          // 1st pixel in line 
    }
  }
  else {
    // -------------- Do left-top fill  -- Position [0]
    dstP = *paddedRawPixels;          // Start at top-left of destination buffer
    neighborID = identifier - fatherSubImage->nbTilesW - 1;
    if( neighborID >= 0) {            // If there is a neighboring tile above
  
      srcTile = tileArr + neighborID;     // Get it's address
      if( srcTile == NULL)          // If the tile doesn't exist
        return FPX_ERROR;         //  then we're in big trouble
      srcTile->TouchRawPixelsBuffer( );   // Set access time stamp
      srcP = srcTile->rawPixels;        // Get the address of it's raw pixels
  
      if( srcP == NULL) {           // If there aren't any raw pixels in memory
        if( (status = srcTile->ReadRawPixels()) != FPX_OK)  // Read-in neighbor tile's raw pixels
          return status;
        srcP = srcTile->rawPixels;
      }
      // Calc where the desired data is located in the source tile and set the read increment
      //  to be a full line of pixels
      srcP += ((srcTile->height - padLen) * srcTile->width);
      srcP += (srcTile->width - padLen);
      srcStride = srcTile->width - padLen;
      
      // Copy the data over
      for( y = 0; y < padLen; y++) {
        for (x = 0; x < padLen; x++)
          *dstP++ = *srcP++;
        srcP += srcStride;
        dstP += padWidth - padLen;
      }
    }
    else {  
      // There is no neighbor tile above and to the left. All we can do is replicate the 
      //  top-left pixel of the target tile to fill this area.
      srcP = rawPixels;           //  so just point to the top line of the target tile
      for( y = 0; y < padLen; y++) {
        for( x = 0; x < padLen; x++) 
          *dstP++ = *srcP;
        dstP += (padWidth - padLen);
      }
    }
  
    // -------------- Do left-middle fill  -- Position [3]
    dstP = *paddedRawPixels + (padWidth * padLen);
    neighborID = identifier - 1;
    
    srcTile = tileArr + neighborID;     // Get it's address
    if( srcTile == NULL)          // If the tile doesn't exist
      return FPX_ERROR;         //  then we're in big trouble
    srcTile->TouchRawPixelsBuffer( );   // Set access time stamp
    srcP = srcTile->rawPixels;        // Get the address of it's raw pixels

    if( srcP == NULL) {           // If there aren't any raw pixels in memory
      if( (status = srcTile->ReadRawPixels()) != FPX_OK)  // Read-in neighbor tile's raw pixels
        return status;
      srcP = srcTile->rawPixels;
    }
    // Calc where the desired data is located in the source tile and set the read increment
    //  to be a full line of pixels
    srcP += (srcTile->width - padLen);
    srcStride = srcTile->width - padLen;
    
    // Copy the data over
    for( y = 0; y < height; y++) {
      for (x = 0; x < padLen; x++)
        *dstP++ = *srcP++;
      srcP += srcStride;
      dstP += (padWidth - padLen);
    }
  
    // -------------- Do left-bottom fill  -- Position [5]
    dstP = *paddedRawPixels + ((height + padLen) * padWidth);
    neighborID = identifier + (fatherSubImage->nbTilesW - 1);
  
    if( neighborID < numTiles) {        // If there is a neighboring tile below
  
      srcTile = tileArr + neighborID;     // Get it's address
      if( srcTile == NULL)          // If the tile doesn't exist
        return FPX_ERROR;         //  then we're in big trouble
      srcTile->TouchRawPixelsBuffer( );   // Set access time stamp
      srcP = srcTile->rawPixels;        // Get the address of it's raw pixels
  
      if( srcP == NULL) {           // If there aren't any raw pixels in memory
        if( (status = srcTile->ReadRawPixels()) != FPX_OK)  // Read-in neighbor tile's raw pixels
          return status;
        srcP = srcTile->rawPixels;
      }
      srcP += (srcTile->width - padLen);
      srcStride = srcTile->width - padLen;
      srcCnt = srcTile->height;
      for( y = 0; y < padLen; y++) {
        for (x = 0; x < padLen; x++)
          *dstP++ = *srcP++;
        dstP += (padWidth - padLen);
        srcCnt--;
        if( srcCnt > 0)           // If there are more lines of src data
          srcP += srcStride;        //  advance to next src line
        else                // Else
          srcP -= padLen;         //  Move back and replicate last line
      }
    }
    else {                    // No tile below. Replicate last pixel
      srcP = dstP - padWidth;         // of last line of target
      for( y = 0; y < padLen; y++) {
        for (x = 0; x < padLen; x++)
          *dstP++ = *srcP;
        dstP += (padWidth - padLen);
      }
    }   
  
  }


  // ------------------  Start right fills
  if( ((identifier + 1) % fatherSubImage->nbTilesW) == 0) {
    // -------------  Do right fills -- Positions [2], [4] and [7]
    // The tile being padded is at the right edge, so all we have to do is to copy/extend 
    //  the rightmost pixels that have already been loaded into the padded buffer to fill
    //  the pad area along the right side.
    dstP = *paddedRawPixels + padWidth - padLen;
    for (y = 0; y < height + (2L * padLen); y++) {
      srcP = dstP - 1;
      for (x = 0; x < padLen; x++)
        *dstP++ = *srcP;
      dstP += (padWidth - padLen);
    }
  }
  else {
    // The tiles to the right may not be as wide as 'padLen'. So the next 2 sections will
    //  copy whatever piel data is available.  At the end of this section the rightmost
    //  pixels in the padded buffer will be replicated, if need be, to fill the remainder.
    long  srcLen = padLen;
    
    // -------------- Do right-top fill  -- Position [2]
    dstP = *paddedRawPixels + (padLen + width);
    neighborID = identifier - fatherSubImage->nbTilesW + 1;
    if( neighborID >= 0) {            // If there is a neighboring tile above
  
      srcTile = tileArr + neighborID;     // Get it's address
      if( srcTile == NULL)          // If the tile doesn't exist
        return FPX_ERROR;         //  then we're in big trouble
      srcTile->TouchRawPixelsBuffer( );   // Set access time stamp
      srcP = srcTile->rawPixels;        // Get the address of it's raw pixels
  
      if( srcP == NULL) {           // If there aren't any raw pixels in memory
        if( (status = srcTile->ReadRawPixels()) != FPX_OK)  // Read-in neighbor tile's raw pixels
          return status;
        srcP = srcTile->rawPixels;
      }

      srcP += ((srcTile->height - padLen) * srcTile->width);
      if( srcLen > srcTile->width)
        srcLen = srcTile->width;
      srcStride = srcTile->width - srcLen;
      
      // Copy the data over
      for( y = 0; y < padLen; y++) {
        for( x = 0; x < srcLen; x++) 
          *dstP++ = *srcP++;
        srcP += srcStride;
        dstP += (padWidth - srcLen);
      }
    }
    else {  
      // There is no neighbor tile above and to the right. All we can do is replicate the 
      //  top-right pixel of the target tile to fill this area.
      srcP = rawPixels + (width - 1);     //  Point to last pixel of top source line tile
      for( y = 0; y < padLen; y++) {
        for( x = 0; x < padLen; x++) 
          *dstP++ = *srcP;
        dstP += (padWidth - padLen);
      }
    }
    
    // -------------- Do right-middle fill  -- Position [4]
    dstP = *paddedRawPixels + (padLen * padWidth) + width + padLen;
    neighborID = identifier + 1;
  
    srcTile = tileArr + neighborID;       // Get it's address
    if( srcTile == NULL)            // If the tile doesn't exist
      return FPX_ERROR;           //  then we're in big trouble
    srcTile->TouchRawPixelsBuffer( );   // Set access time stamp
    srcP = srcTile->rawPixels;          // Get the address of it's raw pixels

    if( srcP == NULL) {             // If there aren't any raw pixels in memory
      if( (status = srcTile->ReadRawPixels()) != FPX_OK)  // Read-in neighbor tile's raw pixels
        return status;
      srcP = srcTile->rawPixels;
    }
    
    if( srcLen > srcTile->width)
      srcLen = srcTile->width;
    srcStride = srcTile->width - srcLen;
    
    // Copy the data over
    for( y = 0; y < height; y++) {
      for( x = 0; x < srcLen; x++) 
        *dstP++ = *srcP++;
      srcP += srcStride;
      dstP += (padWidth - srcLen);
    }
    
    // -------------- Do right-bottom fill  -- Position [7]
    dstP = *paddedRawPixels + ((height + padLen) * padWidth) + padLen + width;
    neighborID = identifier + fatherSubImage->nbTilesW + 1;
    if( neighborID < numTiles) {        // If there is a neighboring tile above
  
      srcTile = tileArr + neighborID;     // Get it's address
      if( srcTile == NULL)          // If the tile doesn't exist
        return FPX_ERROR;         //  then we're in big trouble
      srcTile->TouchRawPixelsBuffer( );   // Set access time stamp
      srcP = srcTile->rawPixels;        // Get the address of it's raw pixels
  
      if( srcP == NULL) {           // If there aren't any raw pixels in memory
        if( (status = srcTile->ReadRawPixels()) != FPX_OK)  // Read-in neighbor tile's raw pixels
          return status;
        srcP = srcTile->rawPixels;
      }

      if( srcLen > srcTile->width)
        srcLen = srcTile->width;
      srcStride = srcTile->width - srcLen;
      
      // Copy the data over
      for( y = 0; y < padLen; y++) {
        for( x = 0; x < srcLen; x++) 
          *dstP++ = *srcP++;
        srcP += srcStride;
        dstP += (padWidth - srcLen);
      }
    }
    else {  
      // There is no neighbor tile below and to the right. All we can do is replicate the 
      //  bottom-right pixel of the target tile to fill this area.
      srcP = rawPixels + (width * height)- 1; //  Point to last pixel of last source line tile
      for( y = 0; y < padLen; y++) {
        for( x = 0; x < srcLen; x++)  // only need to copy 'srcLen' here
          *dstP++ = *srcP;
        dstP += (padWidth - srcLen);
      }
    }

    // As promised, if there was insufficient pixels in the tiles on the right, then
    //  some replication must be performed to fill the remaining pad area on the right.
    if( srcLen != padLen) {
      padLen -= srcLen;
      dstP = *paddedRawPixels + (padWidth - padLen);
      for (y = 0; y < height + (2L * padLen); y++) {
        srcP = dstP - 1;
        for (x = 0; x < padLen; x++)
          *dstP++ = *srcP;
        dstP += (padWidth - padLen);
      }
    }
  }

 RETURN:
  if (wasLocked == false)
    UnLock();
  return status;
}


// ------------------------------------------------------------------------------
// Read jpeg header from image content property set
//
Boolean PTileFlashPix::ReadHeader(PFlashPixFile* filePtr, unsigned char** pJpegHeader, 
      unsigned long* headerSize )
{
  OLEProperty*  aProp;
  OLEBlob     jpegTable; 
  

  // Get jpeg table index
  unsigned char JPEGtableSelector = GET_JPEGTablesIndex(compressionSubtype);

  // Convert the index into property ID for jpeg table
  unsigned long   PID_jpegTableIndex = PID_JPEGTables(JPEGtableSelector);   

  // If jpeg table index is 0, then the header is stored with tile data
  if ( !JPEGtableSelector )
    return TRUE;    
        
  // If no jpeg table for this resolution, read it from image content property set
  if ( !*pJpegHeader ) {  

    // Find the jpeg table specified by JPEGtableSelector in the image content property set
    if (filePtr->GetImageContentProperty (PID_jpegTableIndex, &aProp)) {

      // Get the jpeg table
      if ( !(jpegTable = (BLOB *)(*aProp)) )  
        return FALSE;             

      *headerSize = jpegTable.ReadVT_VECTOR(pJpegHeader); 

      // Set the table index and flag 
      ((PResolutionFlashPix*)fatherSubImage)->compressTableGroup = JPEGtableSelector; 
    }
    else 
      return FALSE; 
  }
  else 
    // If table index is different from this resolution, read it from image content property set
    if ( JPEGtableSelector != ((PResolutionFlashPix*)fatherSubImage)->compressTableGroup ) {

      // Delete header if it already exists
      delete *pJpegHeader;

      // Find the jpeg table specified by JPEGtableSelector in the image content property set

      if (filePtr->GetImageContentProperty (PID_jpegTableIndex, &aProp)) {

        // Get the jpeg table
        if ( !(jpegTable = (BLOB *)(*aProp)) )  
          return FALSE;     

        *headerSize = jpegTable.ReadVT_VECTOR(pJpegHeader); 
      }
      else 
        return FALSE; 

      // Set the jpeg table index for this resolution 
      ((PResolutionFlashPix*)fatherSubImage)->compressTableGroup = JPEGtableSelector; 
    }
  return TRUE; 
}


// ------------------------------------------------------------------------------
// Write jpeg table to image content property set.
//
Boolean PTileFlashPix::WriteHeader(PFlashPixFile* filePtr, unsigned char* jpegHeader, 
    unsigned long headerSize )
{
  OLEProperty*  aProp;
  OLEBlob     jpegTable; 


  // Get jpeg table index and make sure it is in 0 - 255 range
  unsigned char JPEGtableSelector = ((PResolutionFlashPix*)fatherSubImage)->compressTableGroup;
  JPEGtableSelector = (JPEGtableSelector < 0) ? 0 : JPEGtableSelector; 
  JPEGtableSelector = (JPEGtableSelector >255) ? 255 : JPEGtableSelector; 
  

  // If jpeg table index is 0, then the header is stored with tile data
  if ( !JPEGtableSelector )
    return true;          

  // Convert the index into property ID for jpeg table
  unsigned long   PID_jpegTableIndex = PID_JPEGTables(JPEGtableSelector);     

  // If jpeg table for this index doesn't exist, write it
  if ( !filePtr->GetImageContentProperty (PID_jpegTableIndex, &aProp) ) {

    // Copy the stream in jpeg table into blob
    jpegTable.WriteVT_VECTOR(jpegHeader, headerSize);   

    // Save the jpeg table into image content property set
    if (filePtr->SetImageContentProperty (PID_jpegTableIndex, TYP_JPEGTables, &aProp)) {
      *aProp = jpegTable.GetBlob();
    }
    else
      return false; 

    // Save the maximum jpeg table index
    if ( !filePtr->GetImageContentProperty (PID_MaxJPEGTables, &aProp) ) 
      if (filePtr->SetImageContentProperty (PID_MaxJPEGTables, TYP_MaxJPEGTables, &aProp)) 
        *aProp = (long)JPEGtableSelector; 
      else
        return false; 
    else {
      long tempMaxIndex = (long)(*aProp); 
      tempMaxIndex = (tempMaxIndex < JPEGtableSelector) ? JPEGtableSelector : tempMaxIndex; 
      *aProp = (long)tempMaxIndex; 
    }
    filePtr->Commit();
  }
  return true; 
}


// ------------------------------------------------------------------------------
// Does contrast correction
// Note that if there is an opacity component, this routine assumes that the
//  other components have been pre-multiplied.  In this case, the output of the
//  non-opacity component(s) is clamped to opacity. A more proper method would
//  be to reconstruct the original, non-premultiplied component(s) by multiplying
//  by the reciprocal of opacity, then performing the contrast adjustment and
//  then re-applying the opacity. This is not done here dur to the additional
//  multiplies that are required.
// Note that contrast is not (and should not be) applied to the opacity channel.
//
FPXStatus PTileFlashPix::Contrast (  
      double    k,            // Contrast factor (e.g. 1.2 = +20%contrast)
      FPXBaselineColorSpace colorSpace,   // Which channels are which?
      Pixel     *pixels,          // array of 32-bit pixels
      long    count )         // No. of pixels in array
{
  unsigned char   *lookup;
  unsigned char   *red_pixel, *green_pixel, *blue_pixel, *opac_pixel;
  FPXBaselineColorSpace tempSpace;
  Boolean     monochrome;


  // Set up lookup table, if need be
  if (gContrastVal != k) {
    double  p = (float)0.43;    // Center of contrast spread, per FPX specification
    double  contrasted;
    int   i;

    for (i = 0; i < 256; i++) {
      contrasted = p * pow( ((double)i / 256.0) / p, k); 
      if (contrasted >= 1.0)
        contrasted = (float)0.999;    // Truncate to 0... 255
      gContrastLut[i] = (unsigned char)(contrasted * 256.0);
    }
    gContrastVal = (Boolean)k;
  }
  lookup = gContrastLut;  

  // Determine channel #'s in color space 
  // Alpha channel is not contrasted, but if present it's address is loaded for
  //  use in clipping later.
  // Note that the 'red_pixel' pointer is used for a monochrome component
  opac_pixel = 0;
  tempSpace = colorSpace;
  switch(pixelsSpace) {
    case  SPACE_32_BITS_ARGB: // The 24 bits are stored in the LSB part of the long
      opac_pixel  = ((unsigned char*)pixels);
    case  SPACE_32_BITS_RGB: {
      red_pixel   = ((unsigned char*)pixels) + 1;
      green_pixel = ((unsigned char*)pixels) + 2;
      blue_pixel  = ((unsigned char*)pixels) + 3;
      monochrome  = false;
      } break;
      
    case  SPACE_32_BITS_RGBA: {
      red_pixel   = ((unsigned char*)pixels);
      green_pixel = ((unsigned char*)pixels) + 1;
      blue_pixel  = ((unsigned char*)pixels) + 2;
      opac_pixel  = ((unsigned char*)pixels) + 3;
      monochrome  = false;
      } break;

    case  SPACE_32_BITS_AM:   
      opac_pixel = ((unsigned char*)pixels) + 2;
    case  SPACE_32_BITS_M: {
      red_pixel  = ((unsigned char*)pixels) + 3;
      monochrome = true;
      } break;

    case  SPACE_32_BITS_MA: {
      red_pixel  = ((unsigned char*)pixels) + 2;
      opac_pixel = ((unsigned char*)pixels) + 3;
      monochrome = true;
      } break;
    
    case  SPACE_32_BITS_AYCC: 
      opac_pixel  = ((unsigned char*)pixels);
    case  SPACE_32_BITS_YCC: {
      tempSpace = SPACE_32_BITS_ARGB;
      red_pixel   = ((unsigned char*)pixels) + 1;
      green_pixel = ((unsigned char*)pixels) + 2;
      blue_pixel  = ((unsigned char*)pixels) + 3;
      monochrome  = false;    
      } break;
      
    case  SPACE_32_BITS_YCCA: {
      tempSpace = SPACE_32_BITS_RGBA;
      red_pixel   = ((unsigned char*)pixels);
      green_pixel = ((unsigned char*)pixels) + 1;
      blue_pixel  = ((unsigned char*)pixels) + 2;
      opac_pixel  = ((unsigned char*)pixels) + 3;
      monochrome  = false;
      } break;

    default:
      return FPX_COLOR_CONVERSION_ERROR;  // Invalid color space
    }
  
  // If need be, convert from 'colorSpace' into an RGB 'tempSpace'
  if (tempSpace != colorSpace) 
    ConvertPixelBuffer((unsigned char*)(pixels), width * height, colorSpace, tempSpace);

  if (monochrome) {
    if (opac_pixel) {             // If there is an opacity
      while (count--) {           //  component, then the new
        *red_pixel = lookup[*red_pixel];  //  mono value must be clipped
        if (*red_pixel > *opac_pixel)
          *red_pixel = *opac_pixel;
        red_pixel  += 4;
        opac_pixel += 4;
      }
    } else {
      while (count--) {           //  If there is no opacity
        *red_pixel = lookup[*red_pixel];  //  component, then just blast
        red_pixel += 4;           //  through
      }
    }
  } else { // RGB
    if (opac_pixel) {             // If there is an opacity
      while (count--) {           //  values must be clipped to it
        *red_pixel   = lookup[  *red_pixel];
        if (*red_pixel > *opac_pixel)
          *red_pixel = *opac_pixel;
        *green_pixel = lookup[*green_pixel];
        if (*green_pixel > *opac_pixel)
          *green_pixel = *opac_pixel;
        *blue_pixel  = lookup[ *blue_pixel];
        if (*blue_pixel > *opac_pixel)
          *blue_pixel = *opac_pixel;
  
        red_pixel   += 4;
        green_pixel += 4;
        blue_pixel  += 4;
        opac_pixel  += 4;
      }
    } else {
      while (count--) {           // If there is no opacity
        *red_pixel   = lookup[  *red_pixel];//  no clipping is required
        *green_pixel = lookup[*green_pixel];
        *blue_pixel  = lookup[ *blue_pixel];
  
        red_pixel   += 4;
        green_pixel += 4;
        blue_pixel  += 4;
      }
    }
  }
  
  // If need be, convert from an RGB 'tempSpace' back into 'colorSpace'
  if (tempSpace != colorSpace) 
    ConvertPixelBuffer((unsigned char*)(pixels), width * height, tempSpace, colorSpace);
    
  return FPX_OK;
}


//  ----------------------------------------------------------------------------
//  External Functions
//  ----------------------------------------------------------------------------


//  - EOF ----------------------------------------------------------------------

