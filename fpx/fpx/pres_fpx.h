//  ----------------------------------------------------------------------------
//  MODULE    : PResolutionFlashPix 
//  LANGUAGE  : C++
//  CREATOR   : Philippe BOSSUT 
//  CREAT. DATE : Wednesday, March 20, 1996 
//  DESCRIPTION :  
//  COMMENTS  : 
//  SCCSID      : @(#)pres_fpx.h  1.1 11:59:25 18 Dec 1996
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
  #ifndef PResolutionFlashPix_h
  #define PResolutionFlashPix_h
  #ifndef Commun_h
    #include  "common.h"
  #endif
//  ----------------------------------------------------------------------------

//  Includes
//  --------

#ifndef PResolutionLevel_h
  #include  "pr_level.h"
#endif
#ifndef BufferDesc_h
  #include  "buffdesc.h"
#endif
#ifndef FPXBaselineIO_h
  #include "fpxlibio.h"
#endif

//  Constants
//  ---------

//  Types Declarations
//  ------------------

//  Types Definitions
//  -----------------

//  Classes Declarations
//  --------------------

  class OLEStorage;     // to not include OLEStorage
  class OLEHeaderStream;    // to not include OLEHeaderStream.h

  class PResolutionFlashPix;
  class PHierarchicalImage;

//  Classes Definitions
//  -------------------

  // Resolution class. A resolution is composed of several tiles of pixels.
  // Each resolution points to the next which size is 1/4.

  class PResolutionFlashPix : public PResolutionLevel {

    friend class PHierarchicalImage;
    friend class PFileFlashPixIO;
    friend class PTile;
    friend class PTileFlashPix;
    
  public:
        // Constructors and destructor :
        
                 // Create resolution and next resolution in write mode
                 PResolutionFlashPix (PHierarchicalImage* father, long width, long height, long* quelImage);
                 // Create resolution and next resolution in read mode
                 PResolutionFlashPix (PHierarchicalImage* father, long* offset, long id);

        virtual     ~PResolutionFlashPix();
        
        // Public interface: File specific implementation of a FlashPix Resolution Level:
        // ---------------------------------------------------------------------------

        // Writing tools:
        virtual FPXStatus WriteLine (Pixel* pix, short plan = -1);
        virtual FPXStatus WriteRectangle (long x0, long y0, long x1, long y1, Pixel* pix, short plan = -1);
            FPXStatus   FlushModifiedTiles ( );

        // Reading pixels tools:
        virtual FPXStatus   ReadRectangle (long x0, long y0, long x1, long y1, Pixel* pix);
        virtual FPXStatus ReadSampledRectangle(long x0, long y0,long x1, long y1, Pixel* map, short pixelsPerLine, long mapWidth, long mapHeight, Boolean showAlphaChannel = false, float ratio = 0.0);
        virtual FPXStatus Read (long* px, long* py, Pixel* table);
        virtual FPXStatus ReadInterpolated (long* px, long* py, Pixel* table);
        virtual FPXStatus ReadMean (long xi, long yi, Pixel& pixel);
        virtual FPXStatus ReadMeanInterpolated (long xi, long yi, Pixel& pixel);

        // Get information tools
        virtual FPXStatus GetHistogram (long* alpha, long* red, long* green, long* blue, long* brightness, const CorrectLut* correctLut = NULL);
        virtual Boolean   IsOnTheBorder (long xi, long yi);
        virtual FPXStatus SearchPixelTopLeftCorner(long* x1, long* y1, float ratio);
        virtual FPXStatus GetResolutionSizeInfo (long* width, long* height, long* nbTilesWidth, long* nbTilesHeight);

        // Raw data read and write functions
            FPXStatus ReadRawTile (unsigned long whichTile, 
                       FPXCompressionOption*  compressOption, 
                       unsigned char*     compressQuality,
                       long*          compressSubtype,
                       unsigned long*     dataLength,
                       void**         data);
            FPXStatus WriteRawTile (unsigned long whichTile, 
                        FPXCompressionOption  compressOption, 
                        unsigned char     compressQuality,
                        long          compressSubtype,
                        unsigned long     dataLength,
                        void*         data);
        // FlashPix File management:
        // ----------------------
            FPXStatus SetResolutionDescription();           // Create the Section associated to a resolution in the Image Content Property Set
            FPXStatus GetResolutionDescription();           // Get the Section associated to a resolution in the Image Content Property Set
      FPXBaselineColorSpace   GetBaselineSpace();

        OLEHeaderStream* GetSubStreamData();
        OLEHeaderStream* GetSubStreamHdr();
  protected:
        // Protected interface: Implementation of a Resolution Level:
        // ----------------------------------------------------------
        
        // Subimages management
        virtual FPXStatus AllocTilesArray ();
        
        // Reading tools:
        virtual FPXStatus Read ();    // Read the resolution specific information
        virtual FPXStatus ReadInARectangle(Pixel* bufferOut, short pixelsPerLine, short width, short height,
                         const CorrectLut* correctLut = NULL, Boolean useAlphaChannel = false, const CombinMat* combinaisonMatrix = NULL);

        // Writing tools:
        virtual FPXStatus Write ();   // Write the resolution specific information
        virtual FPXStatus Convolution (long x, long y, Pixel* pix, long width, long height);
        virtual FPXStatus DecimateLevel ();

  private:
            FPXStatus   ReadHeaderStream   ();
            FPXStatus   CreateHeaderStream ();
            FPXStatus   UpdateHeaderStream ();

            void    Init();         // Init data with default values (called by constructors)
            
        FPXColorspace colorSpace;       // Color space structure of this resolution
        short     nbChannels;       // Number of channels (1 to 4 in Baseline)
    FPXBaselineColorSpace   baseSpace;        // Easy to use color space description
        Boolean     isICCprofile;     // true if there is an ICC profile for this subimage
        short     ICCprofile;       // Reference to an ICC profile (if any)

        FPXCompressionOption  compression;  // Compression mode used in this image file
        long      compressionSubtype;   // Special compression info
        unsigned char qualityFactor;      // Quality factor
        unsigned char compressTableGroup;   // compression table id
        unsigned char*  jpegHeader;       // JPEG header 
        unsigned long headerSize;       // JPEG header length

        OLEStorage*    subStorage;      // sub-image storage
        OLEHeaderStream* subStreamHdr;      // sub-image stream containing the header
        OLEHeaderStream* subStreamData;     // sub-image stream containing the blocks
  };

//  'inline' Functions
//  ------------------

inline OLEHeaderStream * PResolutionFlashPix::GetSubStreamData()
{
  return subStreamData;
}

inline OLEHeaderStream * PResolutionFlashPix::GetSubStreamHdr()
{
  return subStreamHdr;
}

inline FPXBaselineColorSpace PResolutionFlashPix::GetBaselineSpace()
{
  return baseSpace;
}

//  'extern' Functions
//  ------------------

//  'extern' Variables
//  ------------------

//  ----------------------------------------------------------------------------
  #endif // PResolutionFlashPix_h
//  ----------------------------------------------------------------------------
