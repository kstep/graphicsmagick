//  ----------------------------------------------------------------------------
//  MODULE    : PTile 
//  LANGUAGE  : C 
//  CREATOR   : Philippe BOSSUT 
//  CREAT. DATE : Wednesday, March 13, 1996 
//  DESCRIPTION :  
//    SCCSID      : @(#)ptile.h 1.7 15:18:40 14 Apr 1997
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
  #ifndef PTile_h
  #define PTile_h
  #ifndef Commun_h
    #include  "common.h"
  #endif
//  ----------------------------------------------------------------------------

//  Includes
//  --------
#include <time.h>

#ifndef SystemIVUE_h
  #include  "ri_sys.h"
#endif
#ifndef Compresseur_h
  #include  "fpxcompress.h"    // for TLC_IdCodec
#endif

#ifndef FPXBaselineIO_h
  #include "fpxlibio.h"
#endif

#ifdef _WINDOWS
#include <winbase.h>
#endif

//  Constants
//  ---------

//  Types Declarations
//  ------------------

//  Types Definitions
//  -----------------

//  Classes Declarations
//  --------------------

  class PHierarchicalImage;
  class PResolutionLevel;
  class PTile;

//  Classes Definitions
//  -------------------

  // PTile class. PTiles keep memory of the pixels themselves.
  // BE CAREFULL : PResolutionLevel manipulates Arrays of PTile and uses arithmetic of pointers
  // operations to compute tiles' addresses, consequently, DO NOT ADD SCALAR MEMBERS OR VIRTUAL 
  // FUNCTIONS TO CLASSES INHERITING FROM PTILE. In other words, anything which makes a modification
  // of the size of PTile's derivate class defeats the arithmetic used in PResolutionLevel and
  // crashes the application. If you want to add something to a derivate class, you should in fact
  // add that here in the protected area (I know, it's ugly...). The only thing allowed in derivated
  // classes is to refurbish some of the virtual methods of the public interface.
  // Another better solution would be to use a chained list or to modify the code everywhere the pointer
  // arithmetic is used using a virtual SizeOf() method... To be done in a future version...
  
  class PTile : public PToolkitObject {
  
    friend class PHierarchicalImage;
    friend class PResolutionLevel;

  public:
              PTile();
        virtual   ~PTile();
        
        // Memory management: Used by the context to handle the tiles:
        // -----------------------------------------------------------
  static    Boolean Purge(long* size = NULL, Boolean purgeAll = FALSE); // Release used memory
  static    long  GetPurgeableMemory();               // Compute the amount of memory which can be purged
  static    void  ClearStaticArrays();  // clear all the static arrays still allocated
        long  AllocatePixels();
        long  AllocateRawPixels();
        long  AllocatePixelMemory( Pixel **memAddress);
        void  FreePixelsBuffer( );
        void  FreeRawPixelsBuffer( );
        void  TouchPixelsBuffer( );
        void  TouchRawPixelsBuffer( );
        long  Free (Boolean forced = FALSE, Boolean freeIncomplete = FALSE);  // Free memory allocated to the tile

        // Memory management: Used by others to handle tiles which have pixels in memory:
        // -----------------------------------------------------------
//  static    long    FindOldestTileBuffer( PTile **foundTile, long *isRawPixelsBuffer);  PTCH_101 rep
  static    long    FindOldestTileBuffer( PTile **foundTile, long *isRawPixelsBuffer, long minSize = 16384);  // PTCH_101 new
  static    void    FreeAncientBuffers( long numMinutesOld);
  static    void    FreeAllRawPixels();
  static    void    FlushModifiedTiles();

        // Public interface: Methods used by a PResolutionLevel to access a tile:
        // ----------------------------------------------------------------------
        // These methods should not be derived

        // Read pixels methods:
        FPXStatus ReadRectangleRawData (Pixel* pix, long width, long height, long rowOffset, long x0, long y0);
        FPXStatus ReadRectangle  (Pixel* pix, long width, long height, long rowOffset, long x0, long y0);
        long    InverseAlpha();     // Inverse alpha channel in tile

        // Write pixels methods:
        FPXStatus WriteTile();      // Advanced tile writing : manage automatic decimation        
        FPXStatus DecimateTile();     // Decimation itself
        FPXStatus WriteRectangle (Pixel* pix, long width, long height, long rowOffset, long x0, long y0, short plan = -1);
        FPXStatus Convolution    (Pixel* pix, long width, long height, long quadrant);
            
        // Get information tools
        Boolean GetInfo (long* width, long* height, Typ_Compression* compr);
        long  GetposFic ();     // Give tile position in the file
        long  GetTileSize ();     // Give tile size
        Boolean HasFreshPixels ();    // Indicates tile is modified but not yet saved

        // Public interface: Methods implementing the file specific tile description:
        // --------------------------------------------------------------------------
        // These methods must be derived in most of the cases

        // Read and write a tile:
        virtual FPXStatus Read();         // Read  pixels & rawPixels in file
        virtual FPXStatus ReadRawPixels();    // Read  rawPixels in file
        virtual FPXStatus Write();        // Write pixels in file

        // Set information tools
        virtual void  InitializeCreate (PResolutionLevel* father, long width, long height, long id);  // Init tile in write mode
        virtual void  InitializeRead (PResolutionLevel* father, long offset, long sizetile, long id, long compression = 0, long nouse = 0); // Init tile in read mode

        // Get compression option
        virtual long  GetCompression ();
        virtual long  GetCompressionSubtype ();

        // Set compression option
        virtual void  SetCompression (long theCompression);
        virtual void  SetCompressionSubtype(long theCompressionSubType);
        
        // Set quality factor
        virtual void  SetQualityFactor (unsigned char theQualityFactor);
        
        // Convert the FPX compression code to index of compressor 
        virtual TLC_IdCodec   ConvertCompressionOption (); 

  protected:
        // Data of the PTile structure:
        // ----------------------------

        PResolutionLevel* fatherSubImage;   // Point to the father sub-image

        short     height;         // Height of the tile in pixels
        short     width;          // Width of the tile in pixels
        long      freshPixels;      // Number of pixels recently written into 'rawPixels'
                            //  If freshPixels is non-zero, then the tile in memory 
                            //  has been written to, but not saved in the file yet.
        Pixel*      rawPixels;        // An uncompressed version of data store in file
        Pixel*      pixels;         // A copy of 'rawPixels' with colortwist, etc applied
    FPXBaselineColorSpace rawPixelsSpace;     // Color space for the data currently in 'rawPixels'
    FPXBaselineColorSpace pixelsSpace;      // Color space for the data currently in 'pixels'
                            //  suitable for user display. Color space always 
                            //  the same as  'PFileFlashPixIO::usedSpace'
        time_t      pixelsTime;       // Last time  'pixels'   was accessed
        time_t      rawPixelsTime;      // Last time 'rawPixels' was accessed
          
        short     nbChannels;       // Number of channels (1 to 4 in Baseline)
        long      dirtyCount;       // Check for modification of contrast, colortwist, filtering, or aspect ratio
        Boolean     viewingParamApplied;  // True if the viewing parameters have been applied to the cached tile values
        Boolean     pixelsStale;      // True if 'pixels' does not reflect changes to 'rawPixels'
        
        TLC_IdCodec   idCodec;        // Compressor/Decompressor identifier
        Boolean     decompressorIsMissing;  // True if decompressor is missing
        long      compression;      // Compression mode used in this image file
        long      compressionSubtype;   // Special compression info
        unsigned char qualityFactor;      // Quality factor
  
        Boolean     tileInitialize;     // Flag of tile having been initialized
        long      posPixelFic;      // Position in the file
        long      tileSize;       // Size of the tile in the file
        int32     identifier;       // Tile identifier (i.e. tile number)
        
        // For fast computation in PTile methods
        // -------------------------------------
        
  static    unsigned char*  invertLUT;        // Inversion LUT
        void      AllocInvertTable();   // Allocate and compute the inversion LUT
        
        // For memory management purposes
        // ------------------------------
  
        PTile*      previous;       // Pointer to the previous tile allocated
        PTile*      next;         // Pointer to the next tile allocated
        
        void      Insert();       // Put the tile at the end of the list
        void      Dispose();        // Delete the tile from the list
        void      Lock();         // Disable delete of this tile
        void      UnLock();       // Enable delete of this tile
        Boolean     IsLocked();       // Check if this tile is locked

        void      AllocDecompress (long size);  // Allocate buffer for decompression
  static    long      PurgeDecompress ();       // Free decompression buffer
        void      LockDecompress ();        // Disable the free of decompression buffer
        void      UnLockDecompress ();      // Enable the free of decompression buffer
        
  static    long      allocTiles;       // Number of allocated tiles
  static    long      allocSize;        // Size allocated to the tiles
  static    PTile*      first;          // Oldest tile allocated
  static    PTile*      last;         // Most recent tile allocated
  static    PTile**     locked;         // Locked tiles
  static    long      indexLocked;

  static    Ptr     decompressBuffer;   // Global buffer for decompression
  static    long      decompressSize;     // Size of the decompression buffer
  static    Boolean     decompressLock;     // true if buffer in use

  };
  
  

//  'inline' Functions
//  ------------------

// Get Position of the tile in the file

inline long PTile::GetposFic() {
  return (posPixelFic);
}

inline long PTile::GetTileSize () {
  return tileSize;
}

inline void PTile::LockDecompress () {
  decompressLock = TRUE;
}

inline void PTile::UnLockDecompress () {
  decompressLock = FALSE;
}

inline Boolean PTile::HasFreshPixels () {
  return ((freshPixels > 0)? true : false);
}

inline void PTile::TouchPixelsBuffer () {
#ifdef _WINDOWS
    pixelsTime = GetCurrentTime();
#else
    pixelsTime = clock();
#endif
}

inline void PTile::TouchRawPixelsBuffer () {
#ifdef _WINDOWS
    pixelsTime = GetCurrentTime();
#else
    rawPixelsTime = clock();
#endif
}


//  'extern' Functions
//  ------------------

//  'extern' Variables
//  ------------------

//  ----------------------------------------------------------------------------
  #endif // PTile_h
//  ----------------------------------------------------------------------------
