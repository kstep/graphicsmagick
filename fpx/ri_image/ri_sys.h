//  ------------------------------------------------------------------------------------------------
//
//                     TOOLKIT GLOBAL MANAGEMENT
//
//  MODULE    : SystemIVUE.h
//  LANGUAGE  : C++
//  AUTHOR    : Philippe BOSSUT
//  DATE    : Wenesday 22 December 1993
//  DESCRIPTION : 
//  COMMENT   : This package is a part of the IVUE Toolkit I/O and the FlashPix Baseline I/O
//  SCCSID      : @(#)ri_sys.h  1.2 14:49:49 06 Jan 1997
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
//  ------------------------------------------------------------------------------------------------
  #ifndef SystemToolkit_h
  #define SystemToolkit_h
#ifndef Commun_h
  #include  "common.h"
#endif

//  ------------------------------------------------------------------------------------------------

//  Includes
//  --------

  #include <string.h>

#ifndef FicNom_h
  #include  "filename.h"
#endif
#ifndef FITypes_h
  #include  "b_types.h"
#endif

#ifndef IVUEToolkitIO_h
  #include  "ri_libio.h"
#endif

// This declaration avoids to include FPXBaselineIO.h which causes a lot of problem otherwise
// CAUTION: this declaration has to be maintained identical to the original!
#ifndef tag_FPXProgressFunction
#define tag_FPXProgressFunction
typedef short (* FPXProgressFunction) (long totalToDo, long amountDoneSoFar);
#endif

//  Constants
//  ---------

  #define TOOLKIT_VERSION   1       // Version of the FlashPix Toolkits
  #define TOOLKIT_NAME    "Reference"   // Version name of the FlashPix Toolkits
  #define TOOLKIT_RELEASE   0       // Release of the FlashPix Toolkits
  #define DEFAULT_TILE_SIZE (64*64*4)   // Default size of the FlashPix tiles
  #define MIN_TILE_SIZE   (16*16*4)   // Minimum size of a FlashPix tile
  #define TOOLKIT_MIN_MEMORY  1000000     // Minimum memory size allowed to Toolkits

  // Authorized color spaces in Baseline. See spec Table III.6
  // We'll have one days something else than 32 bits pixel width.
  
  typedef enum {
    SPACE_32_BITS_RGB,  // The 24 bits are stored in the LSB part of the long
    SPACE_32_BITS_ARGB,
    SPACE_32_BITS_RGBA,
    SPACE_32_BITS_YCC,  // The 24 bits are stored in the LSB part of the long
    SPACE_32_BITS_AYCC,
    SPACE_32_BITS_YCCA,
    SPACE_32_BITS_M,    // The 8  bits are stored in the LSB part of the long
    SPACE_32_BITS_AM,   // The 16 bits are stored in the LSB part of the long
    SPACE_32_BITS_MA,   // The 16 bits are stored in the LSB part of the long
    SPACE_32_BITS_O,    // The 8  bits are stored in the LSB part of the long
    NON_AUTHORIZED_SPACE  // We are in a forbidden case here
  } FPXBaselineColorSpace;
          
//  Class Declarations
//  ------------------

  class PToolkitObject;
  class PErrorsList;
  class PSystemToolkit;

  // To avoid include of PRIImage.h
  class PRIImage;

//  Class Definitions 
//  -----------------

  // General object: all the objects in the toolkits derive from it.
  // It redefines new and delete to handle the memory.
  class PToolkitObject {
  public:
        // redefinition of new and delete in order to used the toolkit memory management
        void* operator  new (size_t size);          
        void* operator  new (size_t size, void *p);         
        void operator delete (void *p);
  };
  
  // List of errors and messages
  class PErrorsList : public PToolkitObject {
  public:
                PErrorsList(OSErr err, FicNom file);
                ~PErrorsList();
                
        void      AddErrorToList(OSErr err, FicNom file);
        
        OSErr     errorCode;
        FicNom      fileName;
        PErrorsList*  nextError;
        
    static  short     nbErr;
  };

  // SystemToolkit : Object managing global parameters and option settings.
  // There is a single PSystemToolkit object declared as "global static" in PRIImage.cpp.
  class PSystemToolkit : public PToolkitObject {
  public:
                PSystemToolkit ();
        virtual     ~PSystemToolkit ();
        
        long      SetTileSize (long newTileSize);

        // The 3 following methods are for internal memory management only
        void      AvailableMemory (long* size);         // Returns the memory available
        void      PurgeSystem ();                 // Purge all the memory reserved 
                                        // by SetAllowance
          long      PurgeSystemWithAmount (long memoryToBePurged);  // Purge a requested amount of memory. 
                                          // Returns the memory really purged.
                                          // (could be smaller than requested)
        
        long      SetWaitFunc (Typ_WaitFunc newWaitFunc);
        long      SetProgFunc (FPXProgressFunction newProgFunc);
        
        long      SetUnit (Typ_Unit  newUnit);
        long      GetUnit (Typ_Unit* newUnit);
        
        long      GetToolkitVersion();
        long      GetToolkitRelease();
        
        long      SetInterleaving (Typ_Interleaving newInterleaving);
        
        long      SetAntialias (Typ_Antialias newAntialias);
        long      SetConvolution (Typ_Convolution newConvolution, Typ_Filter newFilter);
        long      SetConvolution (Typ_Convolution newConvolution);
        
        long      PreserveBackground (Boolean preserve);
        long      SetBackgroundColor (Pixel color, FPXBaselineColorSpace colorSpace = SPACE_32_BITS_RGB);
        void      SetUsedColorSpace (FPXBaselineColorSpace colorSpace);
        
        long      SetUserColorCode (Typ_ColorCode newUserColorCode);
        long      SetAlphaChannel (Boolean existAlphaChannel);
        long      SetFileColorCode (Typ_ColorCode newFileColorCode);
        long      SetColorConversion (Typ_ColorCode colorCode, 
                          Typ_ModeltoRGB modeltoRGB,
                          Typ_RGBtoModel RGBtoModel);

        long      SetCompression (Typ_Compression newidCompressor,
                        Typ_Compressor newCompressor,
                        Typ_Decompressor newDecompressor);
        long      SetCompressionHandle (void* newCompressionHandle, long newHandleSize);
        
        long      SetActiveChannel(short newActiveChannel);

        // The following methods are for internal use only
        float     Ratio();
        void      SetToBackground(ptr_Pixel buffer, long width, long height);
        
        void      LockImage (PRIImage* image);
        Boolean     IsLockedImage (PRIImage* image);
        void      AddErrorToList(short message, OSErr err, FicNom file);
        PErrorsList*  GetErrorsList();
        void      DeleteErrorsList();
        OSErr     GetLastErrorFromList( FicNom file);

        void      SetManageOLE(Boolean theManageOLEOption);
        Boolean     GetManageOLE();

        long        tileSize;     // Size in bytes of the Image tiles
        long        tileWidth;      // Width in Pixels of the Image tiles

        Typ_WaitFunc    fnctWaitFunc;   // Pointer to Wait & Break function
        FPXProgressFunction fnctProgFunc;   // Pointer to Wait & Break function

        Typ_Unit      unit;       // Unit used by the user in commands
        float       ratio;        // Size of 1 Unit in mm
        
        Typ_Interleaving  interleaving;   // Global interleaving mode
        
        Typ_Antialias   antialias;      // Global antialiasing quality
        Typ_Convolution   convolution;    // Global convolution quality
        Typ_Filter      filter;       // Pointer to convolution function
        
        Boolean       preserveBackground; // Transparency on images edges
        Pixel       backgroundColor;  // Color of the background
      FPXBaselineColorSpace backgroundSpace;  // Color Space used for the background color
        
        Typ_ColorCode   userColorCode;    // User color model
        Typ_ColorCode   fileColorCode;    // Output file color model
        Boolean       existAlpha;     // Output file alpha channel flag
        Typ_ModeltoRGB    fnctModeltoRGB[8];  // Pointers to Model to RGB   function
        Typ_RGBtoModel    fnctRGBtoModel[8];  // Pointers to RGB   to Model function
        
        Typ_Compression   compression;    // Global compression mode
        Typ_Compressor    fnctCompression;  // Pointer to compression function
        Typ_Decompressor  fnctDecompression;  // Pointer to decompression function
        void*       compressionHandle;  // Pointer to compression option handle
        long        handleSize;     // Size in bytes of the compression handle
        
        short       activeChannel;    // Active Channel : 0.. x ; All = -1
        
        short       tableLog2[1024];  // Log2 table : used for fast subimage level computation

        PRIImage*     lockedImage;    // Pointer to the current Image (avoid purge of this Image)
        PErrorsList*    errorsList;     // Pointer to the current fatal errors list
  
        Boolean       manageOLE;      // True if we are managing OLE
  };

//  Global Functions Declaration : for internal use only
//  ----------------------------

  extern void *FitsNew(size_t /* size */,void *p);
  extern void *FitsNew(size_t size);
  extern void FitsDelete(void *ptr);
  extern bool GtheSystemToolkitInitialized(void);

  // C functions used by the Toolkit (not exported):
  // -----------------------------------------------
  float     Toolkit_ConvertToMm (float x);
  float     Toolkit_ConvertToUnit (float x);
  Boolean     Toolkit_Interleave (Pixel* source, long width, long height);
  Boolean     Toolkit_UnInterleave (Pixel* source, Pixel* dest, long sourceWidth, long sourceHeight,
                   long destWidth, long destHeight);
  Boolean     Toolkit_CopyInterleaved (Pixel* image, long imageWidth, long imageHeight,
              Pixel* rectangle, long rectWidth, long rectHeight, long i0, long j0);
  short     Toolkit_Log2 (long x);
  Typ_Compression Toolkit_Compression();
  Typ_Convolution Toolkit_Convolution();
  short     Toolkit_ActiveChannel();
  Boolean     Toolkit_WriteOnBackground();
  Pixel     Toolkit_BackgroundColor();
  Typ_ColorCode Toolkit_FileColorCode();
  Typ_ColorCode Toolkit_UserColorCode();
  Typ_ModeltoRGB  Toolkit_ModeltoRGB(Typ_ColorCode code);
  Typ_RGBtoModel  Toolkit_RGBtoModel(Typ_ColorCode code);
  long      Toolkit_TileWidth();
  Boolean     Toolkit_IsLockedIVUE (IVUE* ivue);
  
  // C functions used by the FlashPix IO Toolkit (not exported):
  // --------------------------------------------------------

//  'inline' Functions
//  ------------------

  // PSystemToolkit inline functions :
  
  
  inline float PSystemToolkit::Ratio ()
  
  {
    return ratio;
  }

  inline long PSystemToolkit::GetToolkitVersion ()
  
  {
    return(TOOLKIT_VERSION);
  }

  inline long PSystemToolkit::GetToolkitRelease ()
  
  {
    return(TOOLKIT_RELEASE);
  }

  inline long PSystemToolkit::SetInterleaving(Typ_Interleaving newInterleaving)
  
  {
    interleaving = newInterleaving; return(0);
  }

  inline long PSystemToolkit::SetUserColorCode (Typ_ColorCode newUserColorCode)
  
  {
    userColorCode = newUserColorCode; return(0);
  }
  
  inline long PSystemToolkit::SetAlphaChannel (Boolean existAlphaChannel)
  
  {
    existAlpha = existAlphaChannel; return(0);
  }

  inline long PSystemToolkit::SetFileColorCode (Typ_ColorCode newFileColorCode)
  
  {
    fileColorCode = newFileColorCode; return(0);
  }
  
  inline void PSystemToolkit::LockImage (PRIImage* image)
  
  {
    lockedImage = image;
  }
  
  inline long PSystemToolkit::SetActiveChannel (short newActiveChannel)
  
  {
    activeChannel = newActiveChannel; return(0);
  }
  
  inline Boolean PSystemToolkit::IsLockedImage (PRIImage* image)
  
  {
    return (lockedImage == image);
  }
  
  inline PErrorsList* PSystemToolkit::GetErrorsList()

  {
    return errorsList;
  }
  
//  'extern' Variables
//  ------------------

// Pointer to the PSystemToolkit object : THIS OBJECT MUST BE UNIQUE

extern PSystemToolkit* GtheSystemToolkit;

// This is declared only for compatibility with old LP code...
#define IVUE_System GtheSystemToolkit

//  ------------------------------------------------------------------------------------------------
  #endif // SystemToolkit_h
//  ------------------------------------------------------------------------------------------------
