//  ----------------------------------------------------------------------------
//
//                     FPX FILE MANAGEMENT
//
//  MODULE    : FileFlashPixIO
//  LANGUAGE  : C++
//  AUTHOR    : Philippe BOSSUT
//  DATE    : Thursday, December 16th 1993
//  DESCRIPTION :
//  COMMENT   : This package is a part of the FlashPix Toolkit I/O
//  SCCSID          : @(#)f_fpxio.h 1.3 12:36:08 22 Jul 1997
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
//  ----------------------------------------------------------------------------
  #ifndef FileFlashPixIO_h
  #define FileFlashPixIO_h
#ifndef Commun_h
  #include  "common.h"
#endif

//  ----------------------------------------------------------------------------

//  Includes
//  --------

#ifndef PHierarchicalImage_h
  #include  "ph_image.h"
#endif
#ifndef FPXBaselineIO_h
  #include  "fpxlibio.h"
#endif


//  Constants
//  ---------

  #define FPX_TILE_WIDTH 64 // Baseline limitation: handle only 64x64 tiles...
  
//  Class Declarations
//  ------------------

  class PFileFlashPixIO;

  class OLEStorage;     // to not include OLEStorage
  class OLEProperty;      // to not include OLEProperty
  class OLEHeaderStream;    // to not include OLEHeaderStream.h
  
//  Class Definitions
//  -----------------

  // Handle on an FPX Image Object file:
  
  class PFileFlashPixIO : public PHierarchicalImage {
    
  friend class PFlashPixImageView;
  friend class PResolutionLevel;
  friend class PResolutionFlashPix;
  friend class PTile;
  friend class PTileFlashPix;
  friend class PageImage;

  public:
        // Constructors and destructor :
        
                 // Open the file in Create Mode 
                 PFileFlashPixIO (FicNom& refName, 
                         long width, long height, float resolution,
                         FPXBaselineColorSpace baseSpace, unsigned long backColor, 
                         FPXCompressionOption FPXCompressOption, 
                         Boolean baseUncalibrated = FALSE);
                 PFileFlashPixIO (OLEStorage* theOwningStorage, const char* theStorageName, 
                         long width, long height, float resolution,
                         FPXBaselineColorSpace baseSpace, unsigned long backColor, 
                         FPXCompressionOption FPXCompressOption, 
                         Boolean baseUncalibrated = FALSE);
                 // Open the file in Read Mode only
                 PFileFlashPixIO (FicNom& refName, const char* theStorageName = NULL);
                 PFileFlashPixIO (OLEStorage* theOwningStorage, const char* theStorageName);
                 // Open the file in Read/Write mode (but not Create)
                 PFileFlashPixIO (FicNom& refName, const char* theStorageName, mode_Ouverture openMode);
                 PFileFlashPixIO (OLEStorage* theOwningStorage, const char* theStorageName, mode_Ouverture openMode);

        virtual     ~PFileFlashPixIO ();

        // Public interface: Customize a RI Image:
        // ---------------------------------------

        // Path management : see code for comments...
        virtual OSErr ReadPaths  (data_Record** path, long* nbRecords, long* clipPathNum);
        virtual OSErr WritePaths (data_Record*  path, long  nbRecords, long clipPathNum);

        // Special method for FlashPix:
        // -------------------------
            void  GetResolutionSize (short resolution, long* width, long* height);
            void  GetResolutionTileSize (short resolution, long* nbTilesWidth, long* nbTilesHeight);
            long  GetTileWidth();
            
            void  GetResolutionInfo(FPXResolution* FPXResoInfo, Boolean createdOnly = TRUE);
            void  SetResolutionInfo(FPXResolution* FPXResoInfo);

        // Set compression 
        virtual void  SetCompression (Typ_Compression theCompression);
        virtual void  SetCompressionSubType (long theCompressionSubType);
        virtual long  GetCompressionSubType ();
        virtual FPXCompressionOption  GetCompression ();

        // Set quality factor
        virtual void  SetQualityFactor (unsigned char theQualityFactor);

        // Set jpeg compression table index
            void  SetCompressTableGroup (long theCompressTableGroup);

        // CHG_FIX_FLAT_TNAIL - provide access to member nbCreatedResolutions
        virtual long  GetNumCreatedResolutions ( );
        
        // Raw data read and write functions
            FPXStatus ReadRawTile (unsigned long theResolution, unsigned long whichTile, 
                       FPXCompressionOption*  compressOption, 
                       unsigned char*     compressQuality,
                       long*          compressSubtype,
                       unsigned long*     dataLength,
                       void**         data);
            FPXStatus WriteRawTile (unsigned long theResolution, unsigned long whichTile, 
                        FPXCompressionOption  compressOption, 
                        unsigned char     compressQuality,
                        long          compressSubtype,
                        unsigned long     dataLength,
                        void*         data);
            FPXStatus FlushModifiedTiles( );
            
  protected:
        // Protected interface: Customize a Hierarchical Image:
        // ----------------------------------------------------
        
        // File management:
        virtual FPXStatus OpenFile();   // Read or Init header file information
        virtual FPXStatus CloseFile();  // End sub images computation and writing before closing the file
        
        // Resolution hierarchy management:
        virtual FPXStatus InitResolutionLevelsTable();                      // Update pointers array of sub images
        virtual FPXStatus CreateInitResolutionLevelList();                    // Make a sub image list in read or write mode
        virtual FPXStatus CreateEmptyResolutionLevelList();                   // Make a sub image list in create mode
    virtual PResolutionLevel* CreateEmptyResolutionLevel(long width, long height, long* whichImage);  // Make a sub image in create mode 
    virtual PResolutionLevel* CreateInitResolutionLevel(long* offset, long id);           // Make a sub image in read or write mode 

            FPXStatus MakeNewThumbnail(OLEProperty *aProp);
  private:
        // Private init of a PFileFlashPixIO -> this is not a virtual method
            void    Init();

    // CAUTION: volatile values. The caller must create and call OpenFile() before deleting the objects pointed here (tricky...)
    OLEStorage*       owningStorage;      // The owning storage if any
    const char*       storageName;      // The storage name if any

    FPXCompressionOption  FPXCompression;     // Compression mode used in this image file
    long          FPXCompressionSubType;  // Compression option (subsampling, color conversion and jpeg table, etc) used in this image file
    unsigned char     FPXQualityFactor;   // Quality factor in compression used in this image file

    FPXResolutionUnit   fileUnit;       // Unit used to express sizes in this file  
    long          nbCreatedResolutions; // number of resolutions really created in this FPX file      
    Boolean         tilesHasBeenModified; // true if a tile has been written during the session
  };


//  'inline' Functions
//  ------------------

inline void PFileFlashPixIO::SetCompression (Typ_Compression theCompression) { FPXCompression = (FPXCompressionOption)theCompression; }
inline long PFileFlashPixIO::GetCompressionSubType ()   { return FPXCompressionSubType; }
inline FPXCompressionOption PFileFlashPixIO::GetCompression ()  { return FPXCompression; }

// CHG_FIX_FLAT_TNAIL - provide access to member nbCreatedResolutions
inline long PFileFlashPixIO::GetNumCreatedResolutions()   { return nbCreatedResolutions; }

// 'extern' Functions
//  -----------------

// Create a fpx file
FPXStatus CreateImageByFilename (
              FicNom&             fileName,
        unsigned long         width,
              unsigned long         height,
              unsigned long         tileWidth,      // not implemented in Baseline (always 64)
              unsigned long         tileHeight,     // not implemented in Baseline (always 64)
              FPXColorspace         colorspace,
              FPXBackground         backgroundColor,
              FPXCompressionOption  compressOption, 
              FPXImageHandle**      theFPX);

// Open a fpx file
FPXStatus OpenImageByFilename (
              FicNom&        fileName,
              char*            storagePathInFile, // for the moment, only the root storage is considered
              unsigned long*   width,
              unsigned long*   height,
              unsigned long*   tileWidth,
              unsigned long*   tileHeight,
              FPXColorspace*   colorspace,
              FPXImageHandle** theFPX); 
              
FPXStatus CreateImageWithViewByFilename (
              FicNom&              fileName,
              unsigned long          width,
              unsigned long          height,
              unsigned long          tileWidth,       // not implemented in Baseline (always 64)
              unsigned long          tileHeight,      // not implemented in Baseline (always 64)
              FPXColorspace          colorspace,
              FPXBackground          backgroundColor,
              FPXCompressionOption   compressOption,
              FPXAffineMatrix*       affineMatrix,
              FPXContrastAdjustment* contrastValue,
              FPXColorTwistMatrix*   colorTwist,
              FPXFilteringValue*     filteringValue,
              FPXROI*                regionOfInterest,
              FPXResultAspectRatio*  resultAspectRatio,
              FPXImageHandle**       theFPX); 
              
FPXStatus FPX_CreateImageWithViewByStorage (
              IStorage*              owningStorage,
              char*                  storageName,
              unsigned long          width,
              unsigned long          height,
              unsigned long          tileWidth,
              unsigned long          tileHeight,
              FPXColorspace          colorspace,
              FPXBackground          backgroundColor,
              FPXCompressionOption   compressOption,
              FPXAffineMatrix*       affineMatrix,
              FPXContrastAdjustment* contrastValue,
              FPXColorTwistMatrix*   colorTwist,
              FPXFilteringValue*     filteringValue,
              FPXROI*                regionOfInterest,
              FPXResultAspectRatio*  resultAspectRatio,
              FPXImageHandle**       theFPX); 
              
// 'extern' Variables
//  -----------------

//  ----------------------------------------------------------------------------
  #endif // FileFlashPixIO_h
//  ----------------------------------------------------------------------------
