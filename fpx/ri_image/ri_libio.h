//  ----------------------------------------------------------------------------
//
//  This source code is the property of Live Picture, inc.
//
//  MODULE    : ToolkitIVUE
//  LANGUAGE  : C++
//  AUTHOR    : Philippe BOSSUT
//  DATE    : Tuesday, November 23rd 1993
//  DESCRIPTION : 
//  COMMENT   :
//  SCCSID      : @(#)ri_libio.h  1.1 11:46:44 18 Dec 1996
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
//  ----------------------------------------------------------------------------
  #ifndef IVUEToolkitIO_h
  #define IVUEToolkitIO_h
//  ----------------------------------------------------------------------------

//  Includes
//  --------

  struct Pixel;
  class PErrorsList;
  class PSystemToolkit;
  class PHierarchicalImage;

//  Constants
//  ---------

  #define NOT_IMPLEMENTED     -4        // Return value of a not implemented function

  enum  Typ_Unit    { Unit_inch       = 0,    // Inches
                Unit_m          = 1,    // Meters
                Unit_cm         = 2,    // Centimeters
                Unit_mm         = 3};   // Millimeters
  
  // Flag types used in IVUE :

  enum  Typ_Interleaving {  Interleaving_Pixel    = 0,    // Color by pixel
                Interleaving_Line   = 1,    // Color by line  (not yet implemented)
                Interleaving_Channel  = 2};   // Color by channel (not yet implemented)
  
  enum  Typ_Antialias { Antialias_None      = 0,    // Nearest Neighbor subsampling
                Antialias_Fine      = 1,    // Bilinear interpolation
                Antialias_Extra     = 2};   // no longer used

  enum  Typ_Convolution { Convolution_Standard  = 0,    // 4 neighboors average
                Convolution_Gauss   = 1,    // 8 neighboors gaussian
                Convolution_Other   = 2,    // 8 neighboors user defined (not yet implemented)
                Convolution_Cross   = 3};   // 5 neighboors weighted average -> default value
                    
  enum  Typ_ColorCode { ColorCode_RGB     = 0,    // RGB color model
                ColorCode_Other     = 1,    // Unknowed model (not yet implemented)
                ColorCode_YIQ     = 2,    // NTSC YIQ model (not yet implemented)
                ColorCode_YMCK      = 3,    // YMCK color model (not yet implemented)
                ColorCode_YMC     = 4,    // YMC color model  (not yet implemented)
                ColorCode_CIE     = 5,    // CIE color model  (not yet implemented)
                ColorCode_HSV     = 6,    // HSV color model  (not yet implemented)
                ColorCode_HSL     = 7};   // HSL color model  (not yet implemented)
                    
  enum    Typ_Compression {   Compression_None    = 0,    // No compression
                Compression_NeoTech5  = 5,    // NeoTech Cards (no longer used)
                Compression_NeoTech15 = 15,   // NeoTech Cards (no longer used)
                Compression_NeoTech25 = 25,   // NeoTech Cards (no longer used)
                Compression_QuickTime = 1,    // QuickTime
                Compression_32to24    = 2,    // Raw 32 bits to 24 bits 
                Compression_Other   = 3,    // Unidentified compressor  (not yet implemented)
                      Compression_SingleColor = 6,    // FPX Single Color Compression
                      Compression_JPEG    = 7,    // FPX JPEG Compression 
                        Compression_PNG     = 16};    // FPX PNG: not used yet, could be cool for the web...

  // Identifiers of known compressors
  typedef enum {  TLC_Aucun   = 0,    // No compressor
          TLC_NeoTech5  = 5,    // 3 time the same NeoTech card
          TLC_NeoTech15 = 15,
          TLC_NeoTech25 = 25,
          TLC_QuickTime = 1,    // QuickTime
          TLC_32Vers24  = 2,    // Raw 32 bits to 24 bits
          TLC_Other   = 3,    // Unidentified compressor  (not yet implemented)
                TLC_SingleColor = 6,    // FPX Single Color Compression
                TLC_JPEG    = 7,    // FPX JPEG Compression
                  TLC_PNG     = 16,   // FPX PNG: not used yet, could be cool for the web...
                  TLC_INVALID   = -1    // Invalid compression, no compressor for it
          } TLC_IdCodec;

  const int DERNIER_ID_COMPRESSION = TLC_NeoTech25;


  #define ActiveChannel_All   -1                // All Channels are Active
  #define ActiveChannel_Alpha   0               // Only the Alpha channel is active
  #define ActiveChannel_Red   1               // Only the Red channel is active
  #define ActiveChannel_Green   2               // Only the Green channel is active
  #define ActiveChannel_Blue    3               // Only the Blue channel is active
  
  
  // POINTER TYPES TO USER DEFINED FUNCTIONS :
  
  // Pointer to convolution function :            (not yet implemented)
  
  typedef void (* Typ_Filter) (Pixel* source1, Pixel* source2, Pixel* source3, 
                 Pixel* target, long sourcePixelsNumber);

  // Pointers to ModeltoRGB and RGBtoModel functions :    (not yet implemented)
  
  typedef void (* Typ_ModeltoRGB) (Pixel* sourceModel, Pixel* targetRGB, long pixelsNumber);

  typedef void (* Typ_RGBtoModel) (Pixel* sourceRGB, Pixel* targetModel, long pixelsNumber);


  // Pointers to compression and decompression functions :  (not yet implemented) 
  
  typedef Boolean (* Typ_Compressor)    (Pixel* data, short width, short height,
                      Pixel** compressedData, long* compressedSize,
                      void* compressionHandle);

  typedef Boolean (* Typ_Decompressor)  (Pixel* data, short width, short height,
                      Pixel* compressedData, long compressedSize,
                      void* compressionHandle);

  // Pointer to Wait Function :               (not yet implemented)

  typedef Boolean (* Typ_WaitFunc) (long percent);

//  Class declarations 
//  ------------------

  struct IVUE_FileName;
  
#ifdef __MWERKS__
  struct IVUE_FileName : __no_exception_count {
#else
  struct IVUE_FileName {
#endif
#ifdef macintosh
        Str63     name;
        OSType      type;
        long      directory;
        short     volume;
#else
        char      name[256];
#endif
  };

  // Path description :
  #ifndef StdPath_h
  #define StdPath_h
  typedef struct {
        unsigned char   y[4];     // CAUTION : The vertical coordinate 
        unsigned char   x[4];     // comes first in photoshop paths 
  } data_Point;

  typedef struct {
        unsigned short  kind;     // Knot & control points
    union {
        unsigned short  length;
        data_Point    data[3];
    };
  } data_Record;
  #endif

  

//  IVUE Toolkit I/O Class Definitions
//  ----------------------------------

  typedef PErrorsList ErrorsList;
  typedef PSystemToolkit SystemIVUE;
  #define IVUE PHierarchicalImage

//  Class definitions 
//  -----------------

// 'inline' Functions
//  -----------------

// 'extern' Functions
//  -----------------


//  IVUE Toolkit I/O Functions Definitions
//  --------------------------------------

  extern "C" {
     // System management tools : setting options and obtaining global information
     
     // System init and purge
     void IVUE_InitSystem ();
     long IVUE_InitSystemWithMemory (long* newMemorySize);
     void IVUE_PurgeSystem ();
     long IVUE_PurgeSystemWithAmount (long memoryToBePurged);
     void IVUE_ClearSystem ();
     
     // Memory management functions
     long IVUE_SetAllowance (long newMemorySize);
     long IVUE_GetAllowance (long* usedMemory);
     long IVUE_GetAvailableMemory (long* availableMemory);
     long IVUE_GetUsedMemory (long* usedMemory);
     void IVUE_LockIVUE (IVUE* ivue);
     
     // Version and release
     long IVUE_GetToolkitVersion (long* version, long* release);
     
     // Set global options
     long IVUE_SetUnit (Typ_Unit newUnit);
     long IVUE_SetInterleaving (Typ_Interleaving newInterleaving);
     long IVUE_SetAntialias (Typ_Antialias newAntialias);
//     long IVUE_SetConvolution (Typ_Convolution newConvolution, Typ_Filter newFilter);
     long IVUE_SetConvolution (Typ_Convolution newConvolution);
     long IVUE_NoJaggies(Boolean preserve);
     long IVUE_SetBackgroundColor(Pixel color);
     long IVUE_SetUserColorCode (Typ_ColorCode newUserColorCode);
     long IVUE_SetFileColorCode (Typ_ColorCode newFileColorCode);
     long IVUE_SetAlphaChannel (Boolean existAlphaChannel);
     long IVUE_SetColorConversion (Typ_ColorCode colorCode, Typ_ModeltoRGB modeltoRGB,Typ_RGBtoModel RGBtoModel);
     long IVUE_SetCompression (Typ_Compression newidCompressor, Typ_Compressor newCompressor, Typ_Decompressor newDecompressor);
     long IVUE_SetCompressionHandle (void* newCompressionHandle, long newHandleSize);
     long IVUE_SetActiveChannel (short newActiveChannel);
     long IVUE_SetWaitFunc (Typ_WaitFunc newWaitFunc);
     long IVUE_SetTileSize (long newTileSize);
  }

  extern "C" {
     // Open and Close IVUE files tools using a file name path :
     long IVUE_OpenWrite (IVUE** ivue, IVUE_FileName* filName, long width, long height, float resolution);
     long IVUE_OpenRead (IVUE** ivue, IVUE_FileName* filName);
     long IVUE_OpenModif (IVUE** ivue, IVUE_FileName* filName);
     long IVUE_Close (IVUE* ivue);

     // Open and Close IVUE files tools using a file descriptor :
     long IVUE_OpenRefWrite (IVUE** ivue, int fd, long width, long height, float resolution);
     long IVUE_OpenRefRead (IVUE** ivue, int fd);
     long IVUE_OpenRefModif (IVUE** ivue, int fd);
     long IVUE_RefClose (IVUE* ivue);
     
     // Errors handling
     ErrorsList* IVUE_GetErrorsList();
     void IVUE_DeleteErrorsList();
     
     // Basic access tools :
     long IVUE_WriteLine (IVUE* ivue, Pixel* line);
     long IVUE_WriteRectangle (IVUE* ivue, long x0, long y0, long x1, long y1, Pixel* rect);
     long IVUE_ReadRectangle (IVUE* ivue, long x0, long y0, long x1, long y1, Pixel* rect);
     long IVUE_ReadInaRectangle (IVUE* ivue, Pixel* rect, short width, short height, Boolean showAlphaChannel);
     
     // Obtaining usefull information from IVUE file tools : 
     long IVUE_GetFileName (IVUE* ivue, IVUE_FileName* filName);
     long IVUE_GetDimensions (IVUE* ivue, long* width, long* height);
     long IVUE_GetResolution (IVUE* ivue, float* resol);
     long IVUE_GetTileSize (IVUE* ivue, long* tileSize);
     long IVUE_ExistAlphaChannel(IVUE* ivue, Boolean* existAlpha);
     
     // Read and write paths
     long IVUE_ReadPaths  (IVUE* ivue, data_Record** path, long* nbRecords, long* clipPathNum);
     long IVUE_WritePaths (IVUE* ivue, data_Record*  path, long  nbRecords, long  clipPathNum);
     long IVUE_ReleasePaths (IVUE* ivue);
  }
  

//  'inline' Functions 
//  ------------------

//  ----------------------------------------------------------------------------
  #endif // IVUEToolkitIO_h
//  ----------------------------------------------------------------------------
