//  ------------------------------------------------------------------------------------------------
//  MODULE    : Compresseur32Vers24
//  LANGUAGE  : C++
//  AUTHOR    : Laurent SABORET
//  DATE    : Tuesday, September 15th 1992
//  DESCRIPTION : 
//  COMMENT   :
//  SCCSID      : @(#)cp32to24.h  1.1 11:46:38 18 Dec 1996
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
//  ------------------------------------------------------------------------------------------------
  #ifndef Compresseur32Vers24_h
  #define Compresseur32Vers24_h
  #ifndef Commun_h
    #include  "common.h"
  #endif // Commun_h
//  ------------------------------------------------------------------------------------------------

//  Includes
//  --------

  #include  "fpxcompress.h"

//  Constants
//  ---------

//  Classes declarations 
//  --------------------

  class obj_Compresseur32Vers24;
  typedef obj_Compresseur32Vers24* ptr_Compresseur32Vers24;
  typedef obj_Compresseur32Vers24& ref_Compresseur32Vers24;

//  Classes definitions
//  -------------------

  // A obj_Compresseur32Vers24 object expand and compress over 32 bits pixels
  class obj_Compresseur32Vers24 : public obj_Compresseur
  {
  public:
                   obj_Compresseur32Vers24();
    virtual           ~obj_Compresseur32Vers24();

        // Compress an image. Return false on error.
    virtual Boolean       Compresse(Ptr  uncompressedData, short width, short height,
                        Ptr* compressedData, long* compressedSize);
                      
        // Uncompress an image. Return false on error.
    virtual Boolean       Decompresse(Ptr uncompressedData, short width, short height,
                        Ptr compressedData, long compressedSize);

        // Purge data used by the compressor
    virtual Boolean       Purge();
    
        // Set the compressor parameters;
        void        SetCompressionParameters (long nbChan, Boolean leftShift);
    
  private:
        unsigned char*    compressionBuffer;
        long        bufferSize;
        long        nbBytes;
        Boolean       leftShift;
  };

//  Functions 'inline'
//  ------------------

inline void obj_Compresseur32Vers24::SetCompressionParameters (long nbChan, Boolean leftShift)

{
  this->nbBytes = nbChan;
  this->leftShift = leftShift;
}

//  Functions 'extern'
//  ------------------

//  Variables 'extern'
//  ------------------

//  ------------------------------------------------------------------------------------------------
  #endif // Compresseur32Vers24_h
//  ------------------------------------------------------------------------------------------------
