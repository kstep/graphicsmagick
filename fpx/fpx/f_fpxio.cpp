//  ----------------------------------------------------------------------------
//
//                     FlashPix FILE MANAGEMENT
//
//  MODULE    : FileFlashPixIO.cpp
//  LANGUAGE  : C++
//  AUTHOR    : Nicolas ROMANTZOFF
//  DATE    : Wednesday, July 5th 1995
//  DESCRIPTION : 
//  COMMENT   :
//      SCCSID          : @(#)f_fpxio.cpp 1.6 12:47:06 01 Jul 1997
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
  #include "f_fpxio.h"
//  #define Memoire_Debug // doesn't work without our memory management
//  ----------------------------------------------------------------------------

//  Includes
//  --------

  #include  <string.h>
  #include  <stdio.h>

#if defined(USE_LEGACY_INCLUDES)
#  include <fstream.h>
#else
#  include <fstream>
   using namespace std;
#endif
  
#ifdef macintosh
  #include  <Errors.h>
#endif  
#ifndef Numbers_h
  #include  "numbers.h"
#endif
#ifndef Memoire_h
//include "b_memory.h"
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

#ifndef PageIVUE_h
  #include "ri_page.h"
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
#ifndef FlashPixFormat_h
  #include "fpxformt.h"
#endif

#ifndef PResolutionFlashPix_h
  #include "pres_fpx.h"
#endif
#ifndef PTile_h
  #include "ptile.h"
#endif
#ifndef BufferDesc_h
  #include  "buffdesc.h"
#endif
#ifndef FPXThumbnail_h
  #include  "thmbnail.h"
#endif

//  Constants
//  ---------
//
  
  #define WRITING_BUFFER_SIZE (512*1024)

  #define SW 0
  #define SE 1
  #define NW 2
  #define NE 3

  #define NB_MAX_ERREUR       5

//  Variables
//  ---------

//  ----------------------------------------------------------------------------
  #ifdef macintosh
    #pragma segment FileFlashPixIO
  #endif
//  ----------------------------------------------------------------------------

//  ----------------------------------------------------------------------------
//  Internal Functions
//  ----------------------------------------------------------------------------

static long ComputeNumberOfResolutions (long width, long height, long tileWidth)
{
  long nbResolutions = 1;
  
  while ((width > tileWidth) || (height > tileWidth)) {
    nbResolutions++;
    width  = (width  + 1) / 2;
    height = (height + 1) / 2;
  }
  
  return nbResolutions;
}

//  ----------------------------------------------------------------------------
//  External functions
//  ----------------------------------------------------------------------------


//  ----------------------------------------------------------------------------
//  Methods
//  ----------------------------------------------------------------------------

//  ----------------------------------------------------------------------------
//  Methods of the PFileFlashPixIO class
//  
//  Manage all records to an FPX file. The field "firstSubImage" points to the 
//  full resolution image which points of the /4 subimage, and so onÐ
//
//  ----------------------------------------------------------------------------


// Open a file in Read mode only (compressed or uncompressed) :
// CAUTION : the file is opened as long as necessary
PFileFlashPixIO::PFileFlashPixIO (FicNom& refName, const char* theStorageName) : PHierarchicalImage (refName)
{
  Init ();                  // Init the object
  SetTileParameter (FPX_TILE_WIDTH, FPX_TILE_WIDTH);
  storageName = theStorageName;
}


//  ----------------------------------------------------------------------------
PFileFlashPixIO::PFileFlashPixIO (OLEStorage* theOwningStorage, const char* theStorageName) : PHierarchicalImage (0)
{
  Init ();                  // Init the object
  SetTileParameter (FPX_TILE_WIDTH, FPX_TILE_WIDTH);
  owningStorage = theOwningStorage;
  storageName   = theStorageName;
}


//  ----------------------------------------------------------------------------
// Open a file in Read or Write mode (compressed or uncompressed) :
// CAUTION : the file is opened as long as necessary
PFileFlashPixIO::PFileFlashPixIO (FicNom& refName, const char* theStorageName, mode_Ouverture openMode) : PHierarchicalImage (refName)
{
  Init ();                  // Init the object
  SetTileParameter (FPX_TILE_WIDTH, FPX_TILE_WIDTH);
  storageName = theStorageName;
}


//  ----------------------------------------------------------------------------
PFileFlashPixIO::PFileFlashPixIO (OLEStorage* theOwningStorage, const char* theStorageName, mode_Ouverture openMode) : PHierarchicalImage (0, openMode)
{
  Init ();                    // Init the object
  SetTileParameter (FPX_TILE_WIDTH, FPX_TILE_WIDTH);
  owningStorage = theOwningStorage;
  storageName   = theStorageName;
}


//  ----------------------------------------------------------------------------
// Open a file in Create mode (compressed or uncompressed) :
// CAUTION : the file is opened as long as necessary
PFileFlashPixIO::PFileFlashPixIO (FicNom& refName, 
              long width, long height, float resolution,
                FPXBaselineColorSpace baseSpace, unsigned long backColor, 
              FPXCompressionOption FPXCompressOption,
              Boolean baseUncalibrated)         
          : PHierarchicalImage (refName, width, height, resolution)

{
  Init ();                    // Init the object
  
  // Set compression mode. If the image being created is mono or opacity-only,
  //  then clear the "internal color conversion" flags and set the compression 
  //  subtype (i.e., chroma subsample) to (1,1). For these cases, RGB-to-YCC color 
  //  conversion should not be performed, nor should any data be subsampled.
  
  if ((baseSpace == SPACE_32_BITS_AM) || (baseSpace == SPACE_32_BITS_MA)
   || (baseSpace == SPACE_32_BITS_M)  || (baseSpace == SPACE_32_BITS_O)) {
    FPXCompressionSubType &= 0xFFFF00FF;  // Clear chroma subsample flags
    FPXCompressionSubType |= 0x00001100;  //  and insert (1,1)
    FPXCompressionSubType &= 0xFF00FFFF;  // Clear compression subtype flags
  }

  SetCompression((Typ_Compression)FPXCompressOption);
  SetBaseColorSpace(baseSpace);           // Set the native color space of the file
  SetUncalibratedFlag(baseUncalibrated);
  SetBackgroundColor(baseSpace, backColor);     // Set the background color of the file
  SetTileParameter (FPX_TILE_WIDTH, FPX_TILE_WIDTH);
}


//  ----------------------------------------------------------------------------
PFileFlashPixIO::PFileFlashPixIO (OLEStorage* theOwningStorage, const char* theStorageName, 
              long width, long height, float resolution,
              FPXBaselineColorSpace baseSpace, unsigned long backColor, 
              FPXCompressionOption FPXCompressOption,
              Boolean baseUncalibrated) 
          : PHierarchicalImage (0, width, height, resolution)
{
  Init ();                    // Init the object

  // Set compression mode. If the image being created is mono or opacity-only,
  //  then clear the "internal color conversion" flags and set the compression 
  //  subtype (i.e., chroma subsample) to (1,1). For these cases, RGB-to-YCC color 
  //  conversion should not be performed, nor should any data be subsampled.
  
  if ((baseSpace == SPACE_32_BITS_AM) || (baseSpace == SPACE_32_BITS_MA)
   || (baseSpace == SPACE_32_BITS_M)  || (baseSpace == SPACE_32_BITS_O)) {
    FPXCompressionSubType &= 0xFFFF00FF;  // Clear chroma subsample flags
    FPXCompressionSubType |= 0x00001100;  //  and insert (1,1)
    FPXCompressionSubType &= 0xFF00FFFF;  // Clear compression subtype flags
  }

  SetCompression((Typ_Compression)FPXCompressOption);         // Set compression mode
  SetBaseColorSpace(baseSpace);           // Set the native color space of the file
  SetUncalibratedFlag(baseUncalibrated);
  SetBackgroundColor(baseSpace, backColor);     // Set the background color of the file
  SetTileParameter (FPX_TILE_WIDTH, FPX_TILE_WIDTH);
  owningStorage = theOwningStorage;
  storageName   = theStorageName;
}


//  ----------------------------------------------------------------------------
// Close file and clean memory
PFileFlashPixIO::~PFileFlashPixIO()
{
  if (tilesHasBeenModified) {
    // CAUTION : delete sub-images before closing file because
    // some file's information are used to close sub-imagesÐ 
    if (firstSubImage)
      firstSubImage->CloseSubImage();
    // In write mode, we have to commit (write) all changes before deleting anything
    if ((mode != mode_Lecture))
      CloseFile();
  }
}


//  ----------------------------------------------------------------------------
// Init local private stuff
void PFileFlashPixIO::Init()
{
  owningStorage   = NULL;
  storageName   = NULL;
  
  // Set default compression option and subtype
  FPXCompression      = NONE;     // No compression 
  FPXCompressionSubType   = 0x01012200;   // Default setting are interleave, subsampling, color conversion, and 
                      // jpeg table id of 1 
  
  // Set default quality factor
  FPXQualityFactor    = 10; 

  // No resolution created to start with
  nbCreatedResolutions  = 0;
  
  // No tiles written
  tilesHasBeenModified  = false;
  
  // Set default unit
  GtheSystemToolkit->GetUnit((Typ_Unit*)(&fileUnit));
}


//  ----------------------------------------------------------------------------
// Make a sub image in read or write mode
PResolutionLevel* PFileFlashPixIO::CreateInitResolutionLevel(long* offset, long id)
{
  PResolutionFlashPix* ptrResolution = new PResolutionFlashPix(this, offset, id);
  if (ptrResolution)
    ptrResolution->InitWhenReading();
  return ptrResolution;
} 


//  ----------------------------------------------------------------------------
// Make a sub image list in read or write mode and initialize it
FPXStatus PFileFlashPixIO::CreateInitResolutionLevelList()
{
  PResolutionLevel* ssImageTmp;
  long  nbImgs, id, tileHeight;
  long  offset;
  
  if (!nbSubImages) {   // Means that "open" failed 
    RaiseError(FPX_FILE_NOT_OPEN_ERROR);
    return FPX_FILE_NOT_OPEN_ERROR; 
  }
    
  nbImgs = nbSubImages;
  id = 0;

  // Create the first sub image and init it reading the file "filePtr"
  firstSubImage = CreateInitResolutionLevel(&offset, id); 

  ssImageTmp = firstSubImage;
  nbImgs--;
  
  // While the sub image is correct and there is another : create and init the next
  while ((ssImageTmp->Status() == 0) && (nbImgs > 0)) 
  {
    ssImageTmp->next = CreateInitResolutionLevel(&offset, ++id);
    ssImageTmp = ssImageTmp->next;
    nbImgs--;
  }
  if (nbImgs) { // an error has occured during the creation
    RaiseError(FPX_FILE_CREATE_ERROR);
    return FPX_FILE_CREATE_ERROR;
  }
  
  // Get info from the image file
  if (GetInfo (&width, &height, &tileWidth, &tileHeight, &compression))
    RaiseError(FPX_FILE_CREATE_ERROR);
  
  // Update missing parameters 
  cropX0 = 0;
  cropY0 = 0;
  cropX1 = width;
  cropY1 = height;
  hotSpotX = width/2;
  hotSpotY = height/2;
  
  return Status();
} 


//  ----------------------------------------------------------------------------
// Make a sub image in create mode
PResolutionLevel* PFileFlashPixIO::CreateEmptyResolutionLevel(long width, long height, long* whichImage)
{
  PResolutionFlashPix* ptrResolution = new PResolutionFlashPix(this, width, height, whichImage);
  if (ptrResolution)
    ptrResolution->InitWhenCreate();
  return ptrResolution;
} 


//  ----------------------------------------------------------------------------
// Make a sub image list in create mode
FPXStatus PFileFlashPixIO::CreateEmptyResolutionLevelList()
{
  PResolutionLevel* ssImageTmp;
  long        width, height;
  long        nb = 0;
  FPXStatus     err = FPX_OK;
  
  // Compute the number of resolutions
  width  = this->width;
  height = this->height;
  nbSubImages = ComputeNumberOfResolutions (width, height, tileWidth);
  
  // Create the first resolution
  firstSubImage = CreateEmptyResolutionLevel(width, height, &nb); 
  ssImageTmp = firstSubImage;
  
  // While the creation is correct create the next sub image
  while (ssImageTmp->Status() == 0) {
    width  = (width  + 1) / 2;
    height = (height + 1) / 2;
    ssImageTmp->next = CreateEmptyResolutionLevel(width, height, &nb);
    ssImageTmp = ssImageTmp->next;
  }
  
  if (nb == 0) {      // If an error occured during the creation
    RaiseError(FPX_FILE_CREATE_ERROR);
    err = FPX_FILE_CREATE_ERROR;
    nbSubImages = 0;  // Invalid the PFileFlashPixIO object
  }
  
  if (err == FPX_OK)
    err = Status();
  return err;
} 


//  ----------------------------------------------------------------------------
// Open the file, create it if necessary (if mode is write)
FPXStatus PFileFlashPixIO::OpenFile ()
{
  FPXStatus   err = FPX_OK;
  OSErr   fileErr;
  OLEProperty *aProp;
  Boolean   resFromFile = FALSE;
    
  // Open the FPX file
  if (owningStorage) {
    filePtr = new PFlashPixFile (owningStorage, storageName, mode);
  }
  else {
    // Call the generic search function which may be trapped by the different toolkits
    if (mode == mode_Lecture)
      fileName.Search ();
    filePtr = new PFlashPixFile (fileName, storageName, mode);
  }

  // If error while opening : signal error
  if (!filePtr)
    err = FPX_FILE_NOT_OPEN_ERROR;
  else if ((fileErr = ((PFlashPixFile*)filePtr)->Erreur()) != noErr) {
    filePtr->SignaleErreur();
    if (fileErr == badFidErr)
      err = FPX_FILE_NOT_FOUND;
    else 
      err = FPX_FILE_NOT_OPEN_ERROR;
  }
  
  // Read or initialize the number of resolutions
  if ((err == FPX_OK) && (mode != mode_Ecrasement)) {
    // If open successful in read or write but not Create mode :
    // The file is opened and the Image Content Property Set point to the 
    // begining of the stream -> Read the number of resolutions.

    // Number of resolutions (required)
    if (((PFlashPixFile*)filePtr)->GetImageContentProperty (PID_NbResolutions, &aProp))
      nbCreatedResolutions = long(*aProp);
    else 
      err = FPX_FILE_READ_ERROR;
    
    if(nbCreatedResolutions == 1)
      isFlatFile = TRUE;
    else
      isFlatFile = FALSE;
    
    // Full resolution width (required)
    if (((PFlashPixFile*)filePtr)->GetImageContentProperty (PID_HiResWidth, &aProp))
      width = long(*aProp);
    else 
      err = FPX_FILE_READ_ERROR;
    
    // Full resolution height (required)
    if (((PFlashPixFile*)filePtr)->GetImageContentProperty (PID_HiResHeight, &aProp))
      height = long(*aProp);
    else 
      err = FPX_FILE_READ_ERROR;
    
    // Compute the theorical number of resolutions
    nbSubImages = ComputeNumberOfResolutions (width, height, tileWidth);

    // Compute the resolution according to the default size 
    resolution = (float)(300.0/25.4);   // Default resolution
    fileUnit   = FPX_INCHES;        // Default unit
    float widthInUnits, heightInUnits;

    // Units (optionnal)
    if (((PFlashPixFile*)filePtr)->GetImageContentProperty (PID_DefaultDisplUnit, &aProp))
      fileUnit = (FPXResolutionUnit)long(*aProp);

    float convertRatio;
    switch (fileUnit) {
              case FPX_INCHES:    convertRatio = (float)25.4;   break;
              case FPX_METERS:    convertRatio = (float)1000.0;   break;
              case FPX_CENTIMETERS: convertRatio = (float)10.0;   break;
              case FPX_MILLIMETERS: convertRatio = (float)1.0;    break;
    }

    // Height (optionnal)
    if (((PFlashPixFile*)filePtr)->GetImageContentProperty (PID_DefaultDisplHeight, &aProp)) {
      heightInUnits = (float)(*aProp);
      resolution = (float)(height)/(heightInUnits*convertRatio);
      resFromFile = TRUE;
    }

    // Width (optionnal)
    if (((PFlashPixFile*)filePtr)->GetImageContentProperty (PID_DefaultDisplWidth, &aProp)) {
      widthInUnits = (float)(*aProp);
      resolution = (float)(width)/(widthInUnits*convertRatio);
      resFromFile = TRUE;
    }
    if(!resFromFile)
      resolution = (float)height;

    // Path specification Group 

    if (err)
      RaiseError(err);
        
    if ((((PFlashPixFile*)filePtr)->Erreur()) || err) // If an error occurs during reading of information
      RaiseError(err);
  
  } 
  else if (mode == mode_Ecrasement && err == FPX_OK) {
    // In create mode, will have to compute the number of levels to create
    nbSubImages = 0;
  }

  if (!Status() && (GtheSystemToolkit->GetErrorsList() != NULL))
    RaiseError(err);

  if (Status() || (err != FPX_OK)) {
    delete filePtr;     // Close the current file if any
    filePtr = NULL;     // Reset to NULL
    nbSubImages = 0;    // Reset to 0 : means that "open" failed
    mode = mode_Lecture;  // Force mode to be read only
    if (err == FPX_OK)
      err = Status();
  }
  return err;
}


//  ----------------------------------------------------------------------------
// Create the "subImages" array of pointers according to the "firstSubImage" list
FPXStatus PFileFlashPixIO::InitResolutionLevelsTable()
{
  long i; 
  
  FPXStatus err = PHierarchicalImage::InitResolutionLevelsTable();
  if (err)
    return err;

  // Init compression type, quality factor and compression table group 
  for ( i = 0; i < nbSubImages; i++) {
    ((PResolutionFlashPix *)subImages[i])->compression      = FPXCompression;
    ((PResolutionFlashPix *)subImages[i])->qualityFactor    = FPXQualityFactor;
    ((PResolutionFlashPix *)subImages[i])->compressTableGroup = (unsigned char)(nbSubImages - i);
    ((PResolutionFlashPix *)subImages[i])->compressionSubtype   = SET_JPEGTablesIndex(FPXCompressionSubType, (unsigned char)(nbSubImages - i));
  }

  // Resolution Description Groups
  for ( i = 0; i < nbSubImages; i++)  
    if ((err = ((PResolutionFlashPix*)subImages[i])->GetResolutionDescription()) != FPX_OK) {
      break;
    }
    
  // Set the base space being the one of the high res level
  baseSpace = ((PResolutionFlashPix *)subImages[0])->GetBaselineSpace();
  
  return err;
}


//  ----------------------------------------------------------------------------
// Store the header of the file before closing
FPXStatus PFileFlashPixIO::CloseFile()
{
  FPXStatus   err = FPX_OK;
  OLEProperty *aProp;
  OLEProperty *aProp0;
  short     i;

  // Number of resolutions (required)
  if (((PFlashPixFile *)filePtr)->SetImageContentProperty (PID_NbResolutions, TYP_NbResolutions, &aProp))
    *aProp = (long)(nbCreatedResolutions);
  else 
    err = FPX_FILE_WRITE_ERROR;

  // Full resolution width (required)
  if (((PFlashPixFile *)filePtr)->SetImageContentProperty (PID_HiResWidth, TYP_HiResWidth, &aProp))
    *aProp = (long)(width);
  else 
    err = FPX_FILE_WRITE_ERROR;
   
  // Full resolution height (required)
  if (((PFlashPixFile *)filePtr)->SetImageContentProperty (PID_HiResHeight, TYP_HiResHeight, &aProp))
    *aProp = (long)(height);
  else 
    err = FPX_FILE_WRITE_ERROR;

  // Compute the default size according to the resolution 
  float convertRatio;
  switch (fileUnit) {
          case FPX_INCHES:    convertRatio = (float)25.4;   break;
          case FPX_METERS:    convertRatio = (float)1000.0;   break;
          case FPX_CENTIMETERS: convertRatio = (float)10.0;   break;
          case FPX_MILLIMETERS: convertRatio = (float)1.0;    break;
  }
  float widthInUnits  = (float)(width )/(resolution*convertRatio);
  float heightInUnits = (float)(height)/(resolution*convertRatio);

  // Height (optionnal)
  if (((PFlashPixFile*)filePtr)->SetImageContentProperty (PID_DefaultDisplHeight, TYP_DefaultDisplHeight, &aProp))
    *aProp = (float)(heightInUnits);
  else 
    err = FPX_FILE_WRITE_ERROR;
  
  // Width (optionnal)
  if (((PFlashPixFile*)filePtr)->SetImageContentProperty (PID_DefaultDisplWidth, TYP_DefaultDisplWidth, &aProp))
    *aProp = (float)(widthInUnits);
  else 
    err = FPX_FILE_WRITE_ERROR;

  // Unit (optionnal)
  if (((PFlashPixFile*)filePtr)->SetImageContentProperty (PID_DefaultDisplUnit, TYP_DefaultDisplUnit, &aProp))
    *aProp = (long)(fileUnit);
  else 
    err = FPX_FILE_WRITE_ERROR;

  // Create the Resolution Description Groups
  for (i = 0; i < nbCreatedResolutions; i++) {  
    if ((err = (((PResolutionFlashPix*)subImages[i])->SetResolutionDescription())) != FPX_OK) {
      break;
    }
  }
  
  // Create a thumbnail image in summary info property set if it doesn't exist

  if (((PFlashPixFile *)filePtr)->GetSummaryInfoProperty (PID_THUMBNAIL, &aProp0)) {
    if (((PFlashPixFile *)filePtr)->SetSummaryInfoProperty (PID_THUMBNAIL, TYP_THUMBNAIL, &aProp)) {
      err = MakeNewThumbnail ( aProp);
    }
    else
      err = FPX_FILE_READ_ERROR;
  }     
  // Else if image has been changed, write the thumbnail image again
  else if (tilesHasBeenModified) 
    if (((PFlashPixFile *)filePtr)->SetSummaryInfoProperty (PID_THUMBNAIL, TYP_THUMBNAIL, &aProp)) {
      err = MakeNewThumbnail ( aProp);
    }
    else
      err = FPX_FILE_READ_ERROR;
    
  // Create the Compression Information Group

  // Create the Path specification Group
    
  if (err == FPX_OK)
    ((PFlashPixFile*)filePtr)->Commit();
  return err;
}


//  ----------------------------------------------------------------------------
// Create a new thumbnail image and save it in the file
FPXStatus PFileFlashPixIO::MakeNewThumbnail ( OLEProperty *aProp)
{
  FPXStatus err = FPX_OK;                 // PTCH_102

  
  // Determine the width and height of thumbnail
  long  thumbNailWidth, thumbNailHeight; 
  if ( width >= height ) {
    thumbNailWidth = MaxThumbNailSize;
    thumbNailHeight = (long)( thumbNailWidth * height ) / width; 
  }
  else {
    thumbNailHeight = MaxThumbNailSize; 
    thumbNailWidth = (long)( thumbNailHeight * width ) / height; 
  }
  
  // The thumbnail should either be monochrome or 24-bit RGB (never YCC). Save the old
  //  image's colorSpace and force a new one specifically for the thumbnail
  FPXBaselineColorSpace saved_usedSpace = usedSpace;
  if ( (usedSpace == SPACE_32_BITS_MA) || (usedSpace == SPACE_32_BITS_AM)
   ||  (usedSpace == SPACE_32_BITS_M) )
    usedSpace = SPACE_32_BITS_MA;
  else
    usedSpace = SPACE_32_BITS_RGB;

  // Create a buffer to hold the image to be created
  FPXBufferDesc thumbNail( (long)1, thumbNailWidth, thumbNailHeight, usedSpace);
  if (thumbNail.Get32BitsBuffer() == NULL) {
      return FPX_MEMORY_ALLOCATION_FAILED;
  }
  
  // Thumbnails are to be composited on a white background. Save the old background
  //  color and set the current background color to white.
  Pixel   backgroundColor = GtheSystemToolkit->backgroundColor;
  Pixel   whiteColor    = 0xFFFFFFFF;
  GtheSystemToolkit->SetBackgroundColor (whiteColor, SPACE_32_BITS_RGB);
  
  
  // Now render a new thumbnail image
  PageImage page (this, thumbNailWidth, thumbNailHeight, (float)0.0);
  page.ReadPage(thumbNail.Get32BitsBuffer());

  // Create the thumbnail property in the CLIPDATA format
  CLIPDATA  cf; 
  if ( CreateThumbnail((unsigned char *)thumbNail.Get32BitsBuffer(), usedSpace, 
              (short)thumbNailWidth, (short)thumbNailHeight,&cf) ) 
  {
    *aProp = (CLIPDATA *)&cf;
    delete cf.pClipData; 
  }
  else 
    err = FPX_ERROR;
  
  // Reset the used colorSpace and the background color
  usedSpace = saved_usedSpace;
  GtheSystemToolkit->SetBackgroundColor (backgroundColor, SPACE_32_BITS_RGB);

  return err;
}


//  ----------------------------------------------------------------------------
// Write any tiles whose pixels have been modified out to the file.
FPXStatus PFileFlashPixIO::FlushModifiedTiles ( )
{
  long    i;
  FPXStatus   mostRecentStatus, status = FPX_OK;

  for ( i = 0; i < nbSubImages; i++) {
    mostRecentStatus = ((PResolutionFlashPix *)subImages[i])->FlushModifiedTiles();
    if (mostRecentStatus != FPX_OK)
      status = mostRecentStatus;
  }
  return status;
}


//  ----------------------------------------------------------------------------
// Read the tile data from the file without decompression
FPXStatus PFileFlashPixIO::ReadRawTile (
          unsigned long          resolution, 
          unsigned long          whichTile, 
          FPXCompressionOption*  compressOption, 
          unsigned char*         compressQuality,
          long*                  compressSubtype,
          unsigned long*         dataLength,
          void**                 data)
{
  FPXStatus status = FPX_OK;
  
  if (resolution >= (unsigned long) nbSubImages)
    status = FPX_INVALID_RESOLUTION;
  else
    status = ((PResolutionFlashPix*)(subImages[resolution]))->ReadRawTile (whichTile, compressOption, compressQuality, compressSubtype, dataLength, data);

  return status;
}


//  ----------------------------------------------------------------------------
// Write the tile data in the file without compression
FPXStatus PFileFlashPixIO::WriteRawTile (
          unsigned long         resolution, 
          unsigned long         whichTile, 
          FPXCompressionOption  compressOption, 
          unsigned char         compressQuality,
          long                  compressSubtype,
          unsigned long         dataLength,
          void*                 data)
{
  FPXStatus status = FPX_OK;
  
  if (resolution >= (unsigned long) nbSubImages)
    status = FPX_INVALID_RESOLUTION;
  else
    status = ((PResolutionFlashPix*)(subImages[resolution]))->WriteRawTile (whichTile, compressOption, compressQuality, compressSubtype, dataLength, data);

  return status;
}


//  ----------------------------------------------------------------------------
// Read path from the FPX file
// return noErr if everything fine (even if no path)
// path is allocated here with the correct size. It MUST be freed by the caller.
OSErr PFileFlashPixIO::ReadPaths (data_Record** thePath, long* nbRecords, long* theClipPathNum)
{
  // Call the parent: returns noErr if the path is already cached.
  OSErr  err = PHierarchicalImage::ReadPaths (thePath, nbRecords, theClipPathNum);
  if (err == noErr)
    return err;

  // Path not implemented yet in FlashPix
  path        = NULL;
  nbPaths     = 0;
  clipPathNum = 0;

  // Update the path parameters, whatever has been done...
  PHierarchicalImage::ReadPaths (thePath, nbRecords, theClipPathNum);

  return noErr;
}


//  ----------------------------------------------------------------------------
// Write the path in the FPX file
// return noErr if everything fine
OSErr PFileFlashPixIO::WritePaths (data_Record*  thePath, long  nbRecords, long  theClipPathNum)
{
  // Store the path in the cached buffer. Return an error if memory problem.
  OSErr err = PHierarchicalImage::WritePaths (thePath, nbRecords, theClipPathNum);
  if (err != noErr)
    goto end;

  // Do nothing if no path (or absurd negative value)
  if (nbPaths <= 0)
    goto end;
    
  // Path not implemented yet in FlashPix

end:
  return err;
}


//  ----------------------------------------------------------------------------
void PFileFlashPixIO::GetResolutionSize (short resolution, long* width, long* height)
{
  long nbTilesWidth, nbTilesHeight;
  subImages[resolution]->GetResolutionSizeInfo (width, height, &nbTilesWidth, &nbTilesHeight);
}


//  ----------------------------------------------------------------------------
void PFileFlashPixIO::GetResolutionTileSize (short resolution, long* nbTilesWidth, long* nbTilesHeight)
{
  long width, height;
  subImages[resolution]->GetResolutionSizeInfo (&width, &height, nbTilesWidth, nbTilesHeight);
}


//  ----------------------------------------------------------------------------
long PFileFlashPixIO::GetTileWidth()
{
  return tileWidth;
}


//  ----------------------------------------------------------------------------
void PFileFlashPixIO::SetCompressionSubType (long theCompressionSubType) 
{ 
  FPXCompressionSubType = theCompressionSubType; 

  // Set the compression subtype for each resolution 
  for ( long i = 0; i< nbSubImages; i++ ) {
    ((PResolutionFlashPix *)subImages[i])->compressionSubtype = SET_JPEGTablesIndex(FPXCompressionSubType,
                      ((PResolutionFlashPix*)subImages[i])->compressTableGroup);
  }
}


//  ----------------------------------------------------------------------------
void PFileFlashPixIO::SetCompressTableGroup (long theCompressTableGroup) 
{ 
  // Set the jpeg table index for each resolution 
  for ( long i = 0; i< nbSubImages; i++ ) {
    ((PResolutionFlashPix *)subImages[i])->compressTableGroup = (unsigned char) theCompressTableGroup;
    ((PResolutionFlashPix *)subImages[i])->compressionSubtype = SET_JPEGTablesIndex(FPXCompressionSubType,
                      ((PResolutionFlashPix*)subImages[i])->compressTableGroup);
  }
}


//  ----------------------------------------------------------------------------
void PFileFlashPixIO::SetQualityFactor(unsigned char theQualityFactor)
{
  FPXQualityFactor = theQualityFactor; 
  
  // Set the quality factor for each resolution 
  for ( long i = 0; i< nbSubImages; i++ ) 
    ((PResolutionFlashPix *)subImages[i])->qualityFactor = FPXQualityFactor;

}


//  ----------------------------------------------------------------------------
void PFileFlashPixIO::GetResolutionInfo(FPXResolution* FPXResoInfo, Boolean createdOnly)
{
  if (createdOnly && nbCreatedResolutions != 0)
    FPXResoInfo->numberOfResolutions = (short)nbCreatedResolutions; 
  else
    FPXResoInfo->numberOfResolutions = (short)nbSubImages; 

  if ( FPXResoInfo->numberOfResolutions >= FPXMAXRESOLUTIONS ) 
    FPXResoInfo->numberOfResolutions = FPXMAXRESOLUTIONS; 
    
  // Get the resolution info for each subimage 
  for ( long i = 0; i < FPXResoInfo->numberOfResolutions; i++ ) {
    FPXResoInfo->compressionControl[i].compressOption     = ((PResolutionFlashPix*)subImages[FPXResoInfo->numberOfResolutions - 1 - i])->compression;

    // Convert the JPEG's quality factor to Kodak's
    unsigned char qualityFactor = (unsigned char) ((255 - ((PResolutionFlashPix*)subImages[FPXResoInfo->numberOfResolutions - 1 - i])->qualityFactor) / 2.55); 
    FPXResoInfo->compressionControl[i].compressQuality    = qualityFactor;
    FPXResoInfo->compressionControl[i].compressTableGroup = ((PResolutionFlashPix*)subImages[FPXResoInfo->numberOfResolutions - 1 - i])->compressTableGroup;
  }
}


//  ----------------------------------------------------------------------------
void PFileFlashPixIO::SetResolutionInfo(FPXResolution* FPXResoInfo)
{
  FPXResoInfo->numberOfResolutions = (FPXResoInfo->numberOfResolutions <= nbSubImages) ? 
      FPXResoInfo->numberOfResolutions : (short)nbSubImages; 
      
  // Set the resolution info for each subimage 
  for ( long i = 0; i< FPXResoInfo->numberOfResolutions; i++ ) {
    ((PResolutionFlashPix*)subImages[i])->compression       = FPXResoInfo->compressionControl[nbSubImages - 1 - i].compressOption;
    
    // Convert the Kodak's quality factor to JPEG's
    unsigned char qualityFactor = (unsigned char) (2.55 * (100 - FPXResoInfo->compressionControl[nbSubImages - 1 - i].compressQuality)); 
    ((PResolutionFlashPix*)subImages[i])->qualityFactor     = qualityFactor;
    ((PResolutionFlashPix*)subImages[i])->compressTableGroup  = FPXResoInfo->compressionControl[nbSubImages - 1 - i].compressTableGroup;
  
    // Set compression subtype according to table index
    ((PResolutionFlashPix*)subImages[i])->compressionSubtype = SET_JPEGTablesIndex(((PResolutionFlashPix*)subImages[i])->compressionSubtype, 
                        ((PResolutionFlashPix*)subImages[i])->compressTableGroup); 
  }
}
