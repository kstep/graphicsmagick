//  ------------------------------------------------------------------------------------------------
//  MODULE    : Compresseur32Vers24
//  LANGUAGE  : C++
//  AUTHOR    : Laurent SABORET
//  DATE    : Tuesday October 5th 1993
//  DESCRIPTION : 
//  COMMENT   :
//  SCCSID      : @(#)cp32to24.cpp  1.1 11:46:38 18 Dec 1996
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
//  ------------------------------------------------------------------------------------------------
  #include "cp32to24.h"
//  ------------------------------------------------------------------------------------------------

//  Includes
//  --------

  #include  "debug.h"

#ifndef Memoire_h
  #include  "b_memory.h"
#endif

//  Types
//  -----

//  ------------------------------------------------------------------------------------------------
  #ifdef macintosh
    #pragma segment Compresseur32Vers24
  #endif
//  ------------------------------------------------------------------------------------------------

//  ------------------------------------------------------------------------------------------------
//  'intern' functions
//  ------------------------------------------------------------------------------------------------

//  ------------------------------------------------------------------------------------------------
//  Methods
//  ------------------------------------------------------------------------------------------------

obj_Compresseur32Vers24::obj_Compresseur32Vers24 ()

{
  compressionBuffer = NULL;
  bufferSize      = 0;
  compresseurPresent  = decompresseurPresent = TRUE;
  // CAUTION: the default behavior is a classic 3x8 to 4x8 compressor/decompressor
  nbBytes       = 3;
  leftShift     = FALSE;
}

obj_Compresseur32Vers24::~obj_Compresseur32Vers24()

{
  UnLock();
  Purge();
}

Boolean obj_Compresseur32Vers24::Purge()
{
  Boolean aPurge = FALSE;

  if (!compresseurLocked) {     // If compressor not locked
    if (compressionBuffer) {
      FastDeleteArray(compressionBuffer, unsigned char);
      aPurge = TRUE;
    }
    compressionBuffer = NULL;
    bufferSize      = 0;
  }
  
  return aPurge;
}

// Compress.
// Return false if memory error.
Boolean obj_Compresseur32Vers24::Compresse(Ptr  uncompressedData, short width, short height,
                         Ptr* compressedData,   long* compressedSize)
{
  int nbPixels = width * height;
  
  // Security
  *compressedData = NULL;
  *compressedSize = 0;
  
  // Allocate the compression buffer
  if (bufferSize < (nbBytes * nbPixels)) {
    if (compressionBuffer)
      FastDeleteArray(compressionBuffer, unsigned char);
    FastAllocArray(compressionBuffer, unsigned char,nbBytes * nbPixels);
    if (compressionBuffer == NULL) {
      bufferSize = 0;
      return FALSE;
    }
    bufferSize = nbBytes * nbPixels;
  }

  // Update the output values (no more error case after this line)
  *compressedData = (Ptr)compressionBuffer;
  *compressedSize = nbBytes * nbPixels;

  long i, j = 0;
  long offset = 4 - nbBytes;
  unsigned char*  pixelDecompressed = (unsigned char*)(uncompressedData);   // Input Data
  unsigned char*  pixelCompressed   = (unsigned char*)(compressionBuffer);  // Output Data

  // Point to the first significant byte
  if (!leftShift)
    pixelDecompressed += offset;
  
/*
  // THIS CODE DOESN'T WORK ON MICROPROCESSORS WHICH NEED 
  // LONGS ALIGNED ON MULTIPLE OF 4 ADDRESS

  // Fill the buffer (except the last pixel)
  // Copy 4 bytes each time.
  // Rem: don't copy the last pixel, otherwise exceed the input buffer capacity
  for (i = 1; i < nbPixels; ++i) {
    *((long*)pixelCompressed) = *((long*)pixelDecompressed); 
    pixelCompressed   += nbBytes;
    pixelDecompressed += 4;
  }
  
  // Copy the last pixel
  for (i = 0; i < nbBytes; ++i, ++pixelCompressed, ++pixelDecompressed)
    *pixelCompressed = *pixelDecompressed;

*/
  // Copy byte by byte
  for (i = 0; i < nbPixels; ++i) {
    for (j = 0; j < nbBytes; ++j, ++pixelCompressed, ++pixelDecompressed)
      *pixelCompressed = *pixelDecompressed;
    pixelDecompressed += offset;
  }
  
  return TRUE;
}


// Decompress
Boolean obj_Compresseur32Vers24::Decompresse(Ptr uncompressedData,
                                             short width,
                                             short height,
                                             Ptr compressedData,
                                             long compressedSize)
{
  int     nbPixels = width * height;
  long      i, j;
  unsigned char*  pixelCompressed   = (unsigned char*)(compressedData);   // Input Data
  unsigned char*  pixelDecompressed = (unsigned char*)(uncompressedData);   // Output Data
  
  // Check the parameters
  assert(uncompressedData != NULL);
  assert(compressedData   != NULL);
  assert(compressedSize   == nbBytes * nbPixels);

  long offset = 4 - nbBytes;
  
/*
  // THIS CODE DOESN'T WORK ON MICROPROCESSORS WHICH NEED 
  // LONGS ALIGNED ON MULTIPLE OF 4 ADDRESS

  // Erase the first extra bytes if data aligned on the right
  if (!leftShift)
    for (j = 0; j < offset; ++j, ++pixelDecompressed)
      *pixelDecompressed = 0;

  // Fill the uncompressed buffer except for the last pixel
  // Copy 4 bytes at a time from the compress buffer, then erase the pading bytes
  for (i = 1; i < nbPixels; ++i) {
    *((long*)pixelDecompressed) = *((long*)pixelCompressed);
    pixelDecompressed += nbBytes;
    pixelCompressed   += nbBytes;
    for (j = 0; j < offset; ++j, ++pixelDecompressed)
      *pixelDecompressed = 0;
  }
  
  // Copy last pixel
  for (j = 0; j < nbBytes; ++j, ++pixelCompressed, ++pixelDecompressed)
    *pixelDecompressed = *pixelCompressed;
    
  // Erase the last extra bytes if data aligned on the left
  if (leftShift)
    for (j = 0; j < offset; ++j, ++pixelDecompressed)
      *pixelDecompressed = 0;
*/

  // Erase the first extra bytes if data aligned on the right
  if (!leftShift)
    for (j = 0; j < offset; ++j, ++pixelDecompressed)
      *pixelDecompressed = 0;

  // Fill the uncompressed buffer except for the last pixel
  for (i = 1; i < nbPixels; ++i) {
    for (j = 0; j < nbBytes; ++j, ++pixelCompressed, ++pixelDecompressed)
      *pixelDecompressed = *pixelCompressed;
    for (j = 0; j < offset; ++j, ++pixelDecompressed)
      *pixelDecompressed = 0;
  }
  
  // Copy last pixel
  for (j = 0; j < nbBytes; ++j, ++pixelCompressed, ++pixelDecompressed)
    *pixelDecompressed = *pixelCompressed;
    
  // Erase the last extra bytes if data aligned on the left
  if (leftShift)
    for (j = 0; j < offset; ++j, ++pixelDecompressed)
      *pixelDecompressed = 0;
  
  return TRUE;
}

//  ------------------------------------------------------------------------------------------------
//  'extern' functions
//  ------------------------------------------------------------------------------------------------

