//  ------------------------------------------------------------------------------------------------
//  MODULE    : Compressor
//  LANGAGE   : C++
//  AUTHOR    : Adolfo VIDE
//  DATE    : Tuesday, September 15th 1992
//  DESCRIPTION :
//  COMMENT   :
//  SCCSID      : @(#)compress.cpp  1.1 11:46:37 18 Dec 1996
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
//  ------------------------------------------------------------------------------------------------

  #define USE_JPEG

  #include "fpxcompress.h"

//  ------------------------------------------------------------------------------------------------

//  Includes
//  --------

  #include "cp_jpeg.h"
  #include "cp32to24.h"

//  Constants
//  ---------


//  Global variables
//  ------------------

  // lesCodecs is an array of all compressors/decompressors available on the current machine
  ptr_TousLesCodecs tousLesCodecs = NULL;

//  ------------------------------------------------------------------------------------------------
  #ifdef macintosh
    #pragma segment Compresseur
  #endif
//  ------------------------------------------------------------------------------------------------

//  ------------------------------------------------------------------------------------------------
//  Internal C functions
//  ------------------------------------------------------------------------------------------------

//  ------------------------------------------------------------------------------------------------
//  Methods of the class obj_Compresseur
//
//  An obj_Compresseur object is a compressor/decompressor that does nothing
//  ------------------------------------------------------------------------------------------------

obj_Compresseur::~obj_Compresseur()

{
}

Boolean obj_Compresseur::Purge()

{
  return FALSE;
}

Boolean obj_Compresseur::Compresse(Ptr , short , short , Ptr* , long* )
{
  return FALSE;
}

Boolean obj_Compresseur::Decompresse(Ptr , short , short , Ptr , long )
{
  return FALSE;
}


//  ------------------------------------------------------------------------------------------------
//  Methods of the class obj_TousLesCodecs
//
//  An obj_TousLesCodecs object is an array of all compressors/decompressors available on the machine
//  ------------------------------------------------------------------------------------------------

// Initialize the array of compressors lesCodecs[] for this machine
obj_TousLesCodecs::obj_TousLesCodecs()

{
  int i;

  // Allocate the array lesCodecs[]
  lesCodecs = new ptr_Compresseur[DERNIER_ID_COMPRESSION+1];
  if (lesCodecs == NULL)
    return;
  
  // Fills it with 0
  for ( i=0 ; i <= DERNIER_ID_COMPRESSION ; i++ )
    lesCodecs[i] = NULL;
    
    
  // No compression
  lesCodecs[TLC_Aucun]    = new obj_Compresseur;

  // Raw 32 bits to 24 bits
  lesCodecs[TLC_32Vers24]   = new obj_Compresseur32Vers24;

  // FlashPix Single Color Compression
  lesCodecs[TLC_SingleColor]  = new PCompressorSinColor;

  // FlashPix JPEG Compression
  lesCodecs[TLC_JPEG]     = new PCompressorJPEG;
}

// Delete the array of compressors lesCodecs[] (and the compressors themselves)
obj_TousLesCodecs::~obj_TousLesCodecs()

{
  int i;
  
  if (lesCodecs != NULL)
  {
    for ( i=0 ; i <= DERNIER_ID_COMPRESSION ; i++ )
    {
      if (lesCodecs[i] != NULL)
        delete lesCodecs[i];
    }
      
    delete[] lesCodecs;
  }
}

// Purge data used by the compressors
Boolean obj_TousLesCodecs::Purge()

{
  int i;
  Boolean aPurge = FALSE;
  
  if (lesCodecs != NULL) {
    for ( i=0 ; i <= DERNIER_ID_COMPRESSION ; i++ ) {
      if (lesCodecs[i] != NULL)
        aPurge = aPurge || lesCodecs[i]->Purge();
    }
  }
  
  return aPurge;
}

//  ------------------------------------------------------------------------------------------------
//  External C Functions
//  ------------------------------------------------------------------------------------------------

