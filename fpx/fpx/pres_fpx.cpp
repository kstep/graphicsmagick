//  ----------------------------------------------------------------------------
//  MODULE    : PResolutionFlashPix 
//  LANGUAGE  : C++ 
//  CREATOR   : Philippe BOSSUT 
//  CREAT. DATE : Wednesday, March 20, 1996 
//  DESCRIPTION :  
//  COMMENTS  : 
//      SCCSID          : @(#)pres_fpx.cpp  1.6 13:18:52 02 Jun 1997
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------

//  ----------------------------------------------------------------------------
  #include "pres_fpx.h"
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
#ifndef Couleur_h
  #include  "fpx_color.h"
#endif
#ifndef Numbers_h
  #include  "numbers.h"
#endif
#ifndef Memoire_h
//include "b_memory.h"
#endif
#ifndef CorrectLut_h
  #include  "corr_lut.h"
#endif
#ifndef Debug_h
  #include  "debug.h"
#endif

#ifndef FileFlashPixIO_h
  #include "f_fpxio.h"
#endif
#ifndef PTileFlashPix_h
  #include "ptil_fpx.h"
#endif
#ifndef PImageFile_h
  #include "pimgfile.h"
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
#ifndef FlashPixFormat_h
  #include "fpxformt.h"
#endif

//  Constants
//  ---------

//  Variables
//  ---------

//  ----------------------------------------------------------------------------
//  Internal Functions
//  ----------------------------------------------------------------------------

// Convert a FPXColorspace element into a Subimage color data (see FPX Spec Table III.3)
// CAUTION: Algorithm limited to the Baseline spec.

 
static void ComputeChannelColor(DWORD* channelColor, FPXColorspace colorSpace, long index)
{
  // Consistency check
  assert (index < colorSpace.numberOfComponents);

  // Default color space value: this works only in the Baseline environment with the 
  // hypothesis that only Baseline compliant colorspace are given in parameter.
  TypColorSpace space = ColorSpace_Colorless;
  TypColor color = Color_Unspecified;
  
  // Determine the kind of color space coded in colorSpace
  for (long i = 0; i < colorSpace.numberOfComponents; i++) {
    switch (colorSpace.theComponents[i].myColor) {
      case PHOTO_YCC_Y:
        space = ColorSpace_PhotoYCC;
        break;
      case NIFRGB_R:
        space = ColorSpace_NifRGB;
        break;
      case MONOCHROME:
        space = ColorSpace_Monochrome;
        break;
    default:
      {
      }
    }
  }
  
  // Determine the color component coded in colorSpace.theComponents[index]
  switch (colorSpace.theComponents[index].myColor) {
    case PHOTO_YCC_Y:
      color = Color_PhotoYCC_Y;
      break;
    case PHOTO_YCC_C1:
      color = Color_PhotoYCC_C1;
      break;
    case PHOTO_YCC_C2:
      color = Color_PhotoYCC_C2;
      break;
    case NIFRGB_R:
      color = Color_Red;
      break;
    case NIFRGB_G:
      color = Color_Green;
      break;
    case NIFRGB_B:
      color = Color_Blue;
      break;
    case ALPHA:
      color = Color_Opacity;
      break;
    case MONOCHROME:
      color = Color_Monochrome;
      break;
  }

  // Build the color space subfield value
  *channelColor = MakeColorSpec(space,color);
  
  // Handle the calibrated/uncalibrated flag
  if (colorSpace.isUncalibrated)
    SetUncalibrated(*channelColor);
  else
    UnsetUncalibrated(*channelColor);
}


//  ----------------------------------------------------------------------------
// Convert Subimage color data into a FPXColorspace element
// CAUTION: Algorithm limited to the Baseline spec.
static void AnalyseChannelColor(DWORD channelColor, FPXColorspace* colorSpace, long index)
{
  // Check the calibrated/uncalibrated bit
  if (IsUncalibrated(channelColor))
    colorSpace->isUncalibrated = TRUE;
  else
    colorSpace->isUncalibrated = FALSE;

  // Unset the flag bits
  UnsetPremultiplied(channelColor);
  UnsetUncalibrated(channelColor);

  // Analyse the color space according to Baseline rules. See Table III.6 of the FPX spec.
  switch (channelColor) {
    case MakeColorSpec(ColorSpace_Colorless,Color_Opacity):
      colorSpace->theComponents[index].myColor    = ALPHA;
      colorSpace->theComponents[index].myDataType = DATA_TYPE_UNSIGNED_BYTE;
      break;
    case MakeColorSpec(ColorSpace_Monochrome,Color_Monochrome):
      colorSpace->theComponents[index].myColor    = MONOCHROME;
      colorSpace->theComponents[index].myDataType = DATA_TYPE_UNSIGNED_BYTE;
      break;
    case MakeColorSpec(ColorSpace_Monochrome,Color_Opacity):
      colorSpace->theComponents[index].myColor    = ALPHA;
      colorSpace->theComponents[index].myDataType = DATA_TYPE_UNSIGNED_BYTE;
      break;
    case MakeColorSpec(ColorSpace_PhotoYCC,Color_PhotoYCC_Y):
      colorSpace->theComponents[index].myColor    = PHOTO_YCC_Y;
      colorSpace->theComponents[index].myDataType = DATA_TYPE_UNSIGNED_BYTE;
      break;
    case MakeColorSpec(ColorSpace_PhotoYCC,Color_PhotoYCC_C1):
      colorSpace->theComponents[index].myColor    = PHOTO_YCC_C1;
      colorSpace->theComponents[index].myDataType = DATA_TYPE_UNSIGNED_BYTE;
      break;
    case MakeColorSpec(ColorSpace_PhotoYCC,Color_PhotoYCC_C2):
      colorSpace->theComponents[index].myColor    = PHOTO_YCC_C2;
      colorSpace->theComponents[index].myDataType = DATA_TYPE_UNSIGNED_BYTE;
      break;
    case MakeColorSpec(ColorSpace_PhotoYCC,Color_Opacity):
      colorSpace->theComponents[index].myColor    = ALPHA;
      colorSpace->theComponents[index].myDataType = DATA_TYPE_UNSIGNED_BYTE;
      break;
    case MakeColorSpec(ColorSpace_RGB,Color_Red):   // treat unspecified RGB just like NIF RGB
    case MakeColorSpec(ColorSpace_NifRGB,Color_Red):
      colorSpace->theComponents[index].myColor    = NIFRGB_R;
      colorSpace->theComponents[index].myDataType = DATA_TYPE_UNSIGNED_BYTE;
      break;
    case MakeColorSpec(ColorSpace_RGB,Color_Green):   // treat unspecified RGB just like NIF RGB
    case MakeColorSpec(ColorSpace_NifRGB,Color_Green):
      colorSpace->theComponents[index].myColor    = NIFRGB_G;
      colorSpace->theComponents[index].myDataType = DATA_TYPE_UNSIGNED_BYTE;
      break;
    case MakeColorSpec(ColorSpace_RGB,Color_Blue):    // treat unspecified RGB just like NIF RGB
    case MakeColorSpec(ColorSpace_NifRGB,Color_Blue):
      colorSpace->theComponents[index].myColor    = NIFRGB_B;
      colorSpace->theComponents[index].myDataType = DATA_TYPE_UNSIGNED_BYTE;
      break;
    case MakeColorSpec(ColorSpace_RGB,Color_Opacity): // treat unspecified RGB just like NIF RGB
    case MakeColorSpec(ColorSpace_NifRGB,Color_Opacity):
      colorSpace->theComponents[index].myColor    = ALPHA;
      colorSpace->theComponents[index].myDataType = DATA_TYPE_UNSIGNED_BYTE;
      break;
    default:
      assert(false);
      break;
  }
}

//  ----------------------------------------------------------------------------
//  Member Functions
//  ----------------------------------------------------------------------------

//  ----------------------------------------------------------------------------
//  Methods of the PResolutionFlashPix class : this is the Resolution object
//
//  Manage an array on PTileFlashPix objects which contain pixels
//  Each PResolutionFlashPix points to the next which has a resolution 1/4.
//  ----------------------------------------------------------------------------

// Initialize resolution information and create the next sub-image
// Use this constructor in Read or Write mode
PResolutionFlashPix::PResolutionFlashPix (PHierarchicalImage* father, long* offset, long id) :
PResolutionLevel (father, offset, id)
{
  Init();
}


//  ----------------------------------------------------------------------------
// Initialize resolution information and create the next sub-image
// Use this constructor in Create mode
PResolutionFlashPix::PResolutionFlashPix (PHierarchicalImage* father, long width, long height, long* whatImage) :
PResolutionLevel (father, width, height, whatImage)
{
  Init();
}


//  ----------------------------------------------------------------------------
// Clean resolution level and delete all related objects
PResolutionFlashPix::~PResolutionFlashPix()
{
  if (tiles) {
    delete [] tiles;
    tiles = NULL;
  }
  
  // Delete jpeg header if it exists
  if ( jpegHeader )
    delete jpegHeader; 
  
  // CAUTION: delete streams before the storage which contain them
  // Commit() makes sure that everything has been written on the disk (flush
  // all ole buffers for the element) and delete releases the OLE object.

  if (subStreamHdr) {
    subStreamHdr->Commit();
    delete subStreamHdr;
    subStreamHdr = NULL;
  }
  if (subStreamData) {
    subStreamData->Commit();
    delete subStreamData;
    subStreamData = NULL; 
  }
  if (subStorage) {
    subStorage->Commit();
    delete subStorage;
    subStorage  = NULL;
  }
}


//  ----------------------------------------------------------------------------
FPXStatus PResolutionFlashPix::AllocTilesArray ()
{
  tiles = new PTileFlashPix[nbTilesH * nbTilesW];
  return ((tiles == NULL)? FPX_MEMORY_ALLOCATION_FAILED : FPX_OK);
}


//  ----------------------------------------------------------------------------
void PResolutionFlashPix::Init ()
{
  baseSpace     = ((PFileFlashPixIO*)fatherFile)->baseSpace;
  CreateFPXColorSpace (baseSpace, &colorSpace);
  colorSpace.isUncalibrated = ((PFileFlashPixIO*)fatherFile)->baseUncalibrated;
  nbChannels    = colorSpace.numberOfComponents;
  isAlpha     = IsAlphaBaseline(baseSpace);
  premultiplied = true; // always premultiplied by default in Baseline FlashPix
  alphaOffset   = (unsigned char) GetAlphaOffsetBaseline(baseSpace);
  isICCprofile  = false;
  ICCprofile    = 0;

  compression       = ((PFileFlashPixIO*)fatherFile)->FPXCompression; 
  compressionSubtype  = ((PFileFlashPixIO*)fatherFile)->FPXCompressionSubType; 
  qualityFactor     = ((PFileFlashPixIO*)fatherFile)->FPXQualityFactor; 
  compressTableGroup  = GET_JPEGTablesIndex(compressionSubtype);  // Set initial jpeg table id
  jpegHeader      = NULL;     // Set initial jpeg table pointer to null 
  headerSize      = 0;      // Set initial jpeg table length
  
  subStreamHdr  = NULL;
  subStreamData = NULL; 
  subStorage    = NULL; 
}


//  ----------------------------------------------------------------------------
FPXStatus PResolutionFlashPix::CreateHeaderStream ()
{
  // Create the Storage and the Streams to store the data
  PFlashPixFile *fileFPX = (PFlashPixFile*)fatherFile->filePtr;
  FPXStatus   status = FPX_OK;
  char      resolutionName [33];
  char      streamHdrName[33];
  char      streamDataName[33];
  
  // the number associate to the resolution is the inverse than the one use internaly
  // because in FlashPix the resolution 0 is the lowest resolution
  GetResolutionName(resolutionName, fatherFile->nbSubImages - identifier - 1);
  
  // Baseline special: there is only 1 sub image level per resolution and 0 support data
  GetSubImageHdrName(streamHdrName, 0);
  GetSubImageDataName(streamDataName, 0);
  
  CLSID clsID = ID_Resolution;
  if (fileFPX->CreateStorage (clsID, resolutionName, &subStorage)) {
    CLSID clsID = ID_SubImageHdr;
    if (subStorage->CreateHeaderStream (clsID, streamHdrName, &subStreamHdr)) {
      CLSID clsID = ID_SubImageData;
      if (subStorage->CreateHeaderStream (clsID, streamDataName, &subStreamData)) {
        Allocation();
      } else
        status = FPX_FILE_CREATE_ERROR;
    } else
      status = FPX_FILE_CREATE_ERROR;
  } else
    status = FPX_FILE_CREATE_ERROR;
  
  // Update the number of resolution level created for this file
  if (status == FPX_OK)
    ((PFileFlashPixIO*)fatherFile)->nbCreatedResolutions++;
    
  return status;
}   


//  ----------------------------------------------------------------------------
FPXStatus PResolutionFlashPix::ReadHeaderStream ()
{
  // Open the Storage and the Streams to read or write the data
  PFlashPixFile* fileFPX = (PFlashPixFile*)fatherFile->filePtr;
  FPXStatus status = FPX_OK;
  char resolutionName [33];
  char streamHdrName[33];
  char streamDataName[33];
  
  // The number associated to the resolution is the inverse than the one used internaly
  // because in FlashPix the resolution 0 is the lowest resolution
  GetResolutionName(resolutionName, fatherFile->nbSubImages - identifier - 1);
  
  // Baseline special: there is only 1 sub image level per resolution and 0 support data
  GetSubImageHdrName(streamHdrName, 0);
  GetSubImageDataName(streamDataName, 0);
  
  CLSID clsID = ID_Resolution;
  if (fileFPX->OpenStorage (clsID, resolutionName, &subStorage)) {
    CLSID clsID = ID_SubImageHdr;
    if (subStorage->OpenHeaderStream (clsID, streamHdrName, &subStreamHdr)) {
      CLSID clsID = ID_SubImageData;
      if (subStorage->OpenHeaderStream (clsID, streamDataName, &subStreamData))
        status = Read ();         // Create block array and read sub-image information
      else
        status = FPX_FILE_READ_ERROR;
    } else
      status = FPX_FILE_READ_ERROR;
  } else
    status = FPX_FILE_READ_ERROR;
  
  if ((status == FPX_OK) && (tiles == NULL))    // If failed, raise an error
    status = FPX_ERROR;

  if (status)
    imageStatus = status;
  return status;
}   


//  ----------------------------------------------------------------------------
FPXStatus PResolutionFlashPix::UpdateHeaderStream ()
{
  FPXStatus status = FPX_OK;
  
  // Try to read the header stream first
  status = ReadHeaderStream ();

  // If failed, try to create the header stream
  if (status != FPX_OK)
    status = CreateHeaderStream();

  if ((status == FPX_OK) && (tiles == NULL))    // If failed, raise an error
    status = FPX_ERROR;

  if (status)
    imageStatus = status;   
  return status;
}


//  ----------------------------------------------------------------------------
FPXStatus PResolutionFlashPix::GetResolutionDescription()
{
  PFlashPixFile *fileFPX = (PFlashPixFile*)fatherFile->filePtr;
  FPXStatus     status = FPX_OK;
  OLEProperty   *aProp;
  long      FlashPixIdentifier = fatherFile->nbSubImages - identifier - 1;
  
  // SubImage pixel width (required)
  if (fileFPX->GetImageContentProperty (PID_SubImageWidth(FlashPixIdentifier), &aProp))
    realWidth = long(*aProp);
  else 
    status = FPX_FILE_READ_ERROR;
  
  // Subimage pixel heigth (required) 
  if (fileFPX->GetImageContentProperty (PID_SubImageHeight(FlashPixIdentifier), &aProp))
    realHeight = long(*aProp);
  else 
    status = FPX_FILE_READ_ERROR;

  // Subimage color (required)
  if (fileFPX->GetImageContentProperty (PID_SubImageColor(FlashPixIdentifier), &aProp)) {
  
    OLEBlob colorBlob((BLOB*)(*aProp));
    if (colorBlob.GetBlobSize()>0) {

      DWORD tmp, tmp0;
      Boolean uncalibrated = false;
                    
      colorBlob.ReadVT_I4(&tmp);            // Number of subimages
      assert (tmp == 1);                // Only 1 subimage in Baseline
    
      colorBlob.ReadVT_I4(&tmp);            // Number of channels
      assert (tmp <= FPX_MAX_COMPONENTS);       // Baseline limitation: RGB (or YCC) + alpha
      colorSpace.numberOfComponents = (short)(tmp);
      nbChannels =  (short)(tmp);
    
      colorBlob.ReadVT_I4(&tmp0);           // Color of channel 0
      premultiplied = IsPremultiplied(tmp0);
      
      uncalibrated = IsUncalibrated(tmp0);              
      if( uncalibrated == true )                
        ((PFileFlashPixIO*)fatherFile)->SetUncalibratedFlag(uncalibrated);

      AnalyseChannelColor(tmp0,&colorSpace,0);
      ExtractColorSpace(tmp0);
    
      for (long i = 1; i < colorSpace.numberOfComponents; i++) {
        colorBlob.ReadVT_I4(&tmp);          // Color of channel i
        AnalyseChannelColor(tmp,&colorSpace,i);
        premultiplied |= IsPremultiplied(tmp);
        assert ((ExtractColorSpace(tmp)) == tmp0);  // The color space part must be the same for all channels
      // CAUTION: we don't analyse extra subimages (Baseline limitation)
      }
    
      // Update the easy to use Baseline description...
      baseSpace = AnalyseFPXColorSpace(colorSpace);
      assert (baseSpace != NON_AUTHORIZED_SPACE);
      isAlpha     = IsAlphaBaseline(baseSpace);
      alphaOffset = (unsigned char) GetAlphaOffsetBaseline(baseSpace);
    } else 
      status = FPX_FILE_READ_ERROR;
    
  } else 
    status = FPX_FILE_READ_ERROR;

  // Subimage numerical format (required)
  if (fileFPX->GetImageContentProperty (PID_SubImageNumFormat(FlashPixIdentifier), &aProp)) {
    FPXLongArray numericalFormat;
    numericalFormat = (FPXLongArray)(*aProp);
    assert (numericalFormat.length == 1);   // Only one subimage in Baseline
    assert (*(numericalFormat.ptr) == VT_UI1);  // Always 8-bit unsigned integer in Baseline
  } else 
    status = FPX_FILE_READ_ERROR;

  // Decimation method (required)
  if (fileFPX->GetImageContentProperty (PID_DecimationMethod(FlashPixIdentifier), &aProp)) {
    DWORD decimation;
    decimation = long(*aProp);
    switch (decimation) {
      case 0:
        // For the high resolution level, there is no decimation and it's the only case...
        assert (identifier == 0);
        break;
      case 2:           // 2x2 averaging
        fatherFile->convolution = Convolution_Standard;
        break;
      case 4:           // 4x4 gaussian filter
        fatherFile->convolution = Convolution_Gauss;
        break;
  //    case DecimationStandard:  // Default filter (still to be defined...)
  //      fatherFile->convolution = Convolution_FlashPix;
  //      break;
      default:
        // Not supported decimation method
        // djk 14Apr1997 AI#18295: removed assert(false)
        fatherFile->convolution = Convolution_Standard;
        break;
    }
  } else
    status = FPX_FILE_READ_ERROR;
  
  // Decimation prefilter width (optional)
  
  // Subimage attributes (optional)
  // NOT SPECIFIED
  
  // Subimage ICC profile (optional)
  
  // Support data contents (optional)
  // NOT SPECIFIED
  
  return status;
}


//  ----------------------------------------------------------------------------
FPXStatus PResolutionFlashPix::SetResolutionDescription()
{
  PFlashPixFile *fileFPX = (PFlashPixFile*)fatherFile->filePtr;
  FPXStatus     status = FPX_OK;
  OLEProperty   *aProp;
  long      FlashPixIdentifier = fatherFile->nbSubImages - identifier - 1;
  
  // SubImage pixel width (required)
  if (fileFPX->SetImageContentProperty (PID_SubImageWidth(FlashPixIdentifier) , TYP_SubImageWidth, &aProp)) {   // Subimage width
    *aProp = long(realWidth); 
  } else
    status = FPX_FILE_WRITE_ERROR;
  
  // Subimage pixel heigth (required) 
  if (fileFPX->SetImageContentProperty (PID_SubImageHeight(FlashPixIdentifier), TYP_SubImageHeight, &aProp)) {  // Subimage heigth
    *aProp = long(realHeight); 
  } else 
    status = FPX_FILE_WRITE_ERROR;
  
  // Subimage color (required)
  if (fileFPX->SetImageContentProperty (PID_SubImageColor(FlashPixIdentifier), TYP_SubImageColor, &aProp)) {
    OLEBlob colorBlob(8 + 4*colorSpace.numberOfComponents);
    if (colorBlob.GetBlobSize()>0) {
      DWORD tmp = 1;
      colorBlob.WriteVT_I4(tmp);    // Number of subimages (only 1 subimage in Baseline...)
      tmp = (DWORD)(colorSpace.numberOfComponents);
      colorBlob.WriteVT_I4(tmp);    // Number of channels
      for (long i = 0; i < colorSpace.numberOfComponents; i++) {
        ComputeChannelColor(&tmp,colorSpace,i);   // Color of channel i
        if (isAlpha) {
          if (colorSpace.theComponents[i].myColor == ALPHA)
            UnsetPremultiplied(tmp);
          else if (premultiplied)
            SetPremultiplied(tmp);
          else
            UnsetPremultiplied(tmp);
        }
        colorBlob.WriteVT_I4(tmp);
        // CAUTION: we don't analyse extra subimages (Baseline limitation)
      }
      *aProp = (BLOB*)(colorBlob.GetBlob()); 
    } else 
      status = FPX_FILE_WRITE_ERROR;
  } else 
    status = FPX_FILE_WRITE_ERROR;

  // Subimage numerical format (required)
  if (fileFPX->SetImageContentProperty (PID_SubImageNumFormat(FlashPixIdentifier), TYP_SubImageNumFormat, &aProp)) {  // Subimage numerical format
    unsigned long format = VT_UI1;  // Always 8-bit unsigned integer in Baseline
    FPXLongArray numericalFormat;
    numericalFormat.length = 1;   // Only one subimage in Baseline
    numericalFormat.ptr = &format;
    *aProp = (FPXLongArray)(numericalFormat); 
  } else 
    status = FPX_FILE_WRITE_ERROR;
  
  // Decimation method (required)
  if (fileFPX->SetImageContentProperty (PID_DecimationMethod(FlashPixIdentifier), TYP_DecimationMethod, &aProp)) {  // Subimage decimation method
    DWORD decimation;
    switch (fatherFile->convolution) {
      case Convolution_Standard:      // 2x2 averaging
        decimation = 2;
        break;
      case Convolution_Gauss:       // 4x4 gaussian filter
        decimation = 4;
        break;
  //    case Convolution_FlashPix:        // Recommended filter (still to be defined...)
  //      decimation = DecimationStandard;
  //      break;
    default:
      {
      }
    }
    if (identifier == 0)          // For the high resolution level, there is no decimation
      decimation = 0;
    *aProp = long(decimation); 
  } else 
    status = FPX_FILE_WRITE_ERROR;
  
  // Decimation prefilter width (optionnal)
  
  // Subimage attributes (optionnal)
  // NOT SPECIFIED
  
  // Subimage ICC profile (optionnal)
  
  // Support data contents (optionnal)
  // NOT SPECIFIED
  
  return status;
}


//  ----------------------------------------------------------------------------
// Read a resolution level and initialize the buffer 'pixels'
FPXStatus PResolutionFlashPix::Read ()
{
  long    i;
  DWORD     tmp;
  DWORD   n;
  FPXStatus status = FPX_OK;
  DWORD   lengthInfoStream, tileWidth, tileHeight, nbChannel, headersTableOffset;
  DWORD   tileHeadersize, offsetTile, sizeTile, compressType;
  
  // The stream must be opened or created before calling Read()...
  if (subStreamHdr == NULL)
    return FPX_FILE_NOT_OPEN_ERROR;

  if (subStreamHdr->Seek(0)) {    // Position to the beginning of the header stream
  
    // Read the Subimage Header Stream
    if (subStreamHdr->ReadVT_I4(&lengthInfoStream) == 0)  // length of the info stream
      status = FPX_FILE_READ_ERROR;
    if (subStreamHdr->ReadVT_I4(&tmp) == 0)         // Width of sub-image in pixels
      status = FPX_FILE_READ_ERROR;
    realWidth = tmp;
    if (subStreamHdr->ReadVT_I4(&tmp) == 0)         // Height of sub-image in pixels
      status = FPX_FILE_READ_ERROR;
    realHeight = tmp;
    if (subStreamHdr->ReadVT_I4(&n) == 0)         // nb of tiles in the sub-image
      status = FPX_FILE_READ_ERROR;
    if (subStreamHdr->ReadVT_I4(&tileWidth) == 0)     // Tiles width
      status = FPX_FILE_READ_ERROR;
    if (subStreamHdr->ReadVT_I4(&tileHeight) == 0)      // Tiles height
      status = FPX_FILE_READ_ERROR;
    if (subStreamHdr->ReadVT_I4(&nbChannel) == 0)     // number of channels
      status = FPX_FILE_READ_ERROR;
    if (subStreamHdr->ReadVT_I4(&headersTableOffset) == 0)  // Tile headers table offset
      status = FPX_FILE_READ_ERROR;
    if (subStreamHdr->ReadVT_I4(&tileHeadersize) == 0)    // Tile Header length
      status = FPX_FILE_READ_ERROR;

    assert(tileHeadersize == 16); // Should be 16 in Baseline...
    tileHeadersize = 16;

    // Compute numbers of tiles in the width and the height of the resolution level
    nbTilesH = (short) (realHeight/tileHeight + (realHeight%tileHeight ? 1 : 0));
    nbTilesW = (short) (realWidth /tileWidth  + (realWidth %tileWidth  ? 1 : 0));

    // Test values consistency
    if ((!realHeight) || (!realWidth) || (!nbTilesH) || (!nbTilesW))
      n = 0;

    long offset = headersTableOffset;               // Position to the tile table

    if (status == FPX_OK) {
      AllocTilesArray ();
      for (i = 0; i < (long) n; i++) {
        if (subStreamHdr->Seek(offset) == false)        // Offset of each tile of the sub-image in the file
          status = FPX_FILE_READ_ERROR;
        if (subStreamHdr->ReadVT_I4(&offsetTile) == 0)      // Offset of each tile of the sub-image in the file
          status = FPX_FILE_READ_ERROR;
        if (subStreamHdr->ReadVT_I4(&sizeTile) == 0)      // Size of each tile
          status = FPX_FILE_READ_ERROR;
        if (subStreamHdr->ReadVT_I4(&compressType) == 0)    // Compression type
          status = FPX_FILE_READ_ERROR;
        if (subStreamHdr->ReadVT_I4((DWORD *)&compressionSubtype) == 0)   // Sub-compression type
          status = FPX_FILE_READ_ERROR;

        // Break and exit if error
        if (status != FPX_OK)
          break;
        
        compression = (FPXCompressionOption)compressType; 
        tiles[i].InitializeRead(this,offsetTile,sizeTile,i, compressType, compressionSubtype);
        offset += tileHeadersize;
      }
    }
  }
  
  // Error handling
  if (status != FPX_OK) {
    fatherFile->filePtr->SignaleErreur();
    delete [] tiles;
    tiles = NULL;
    realHeight = 0;
    realWidth  = 0;
    nbTilesH   = 0;
    nbTilesW   = 0;
  }

  return (status);
}


//  ----------------------------------------------------------------------------
// Write the tile header of a sub-image :
FPXStatus PResolutionFlashPix::Write()
{
  register long   TILE_WIDTH = fatherFile->tileWidth;
  FPXStatus   status = FPX_OK;

  // If nothing has never been writen or read from this resolution level,
  // there's no point to save anything in the subStreamHdr...
  if (!HasBeenUsed())
    return FPX_OK;

  if (subStreamHdr->Seek(0)) {  // Position to the beginning of the header stream 
  
    DWORD offset = 36;              // 9 fields of 4 bytes each in this version...
    DWORD n = nbTilesH * nbTilesW;        // number of tiles
    DWORD tileWidth = TILE_WIDTH;       // Fixed and equal to 64 in Baseline...
    DWORD nbChannel = GetNbChannel(baseSpace);  // Number of channels
    DWORD length = 16;              // 4 fileds of 4 bytes each in this version...
    DWORD tmp;

    // Write the SubImage Header Stream header
    if (subStreamHdr->WriteVT_I4(&offset) == 0)   // Length of header stream header
      status = FPX_FILE_WRITE_ERROR;
    tmp = realWidth;
    if (subStreamHdr->WriteVT_I4(&tmp) == 0)    // Width of the sub-images in pixels
      status = FPX_FILE_WRITE_ERROR;
    tmp = realHeight;
    if (subStreamHdr->WriteVT_I4(&tmp) == 0)    // Height of the sub-images in pixels
      status = FPX_FILE_WRITE_ERROR;
    if (subStreamHdr->WriteVT_I4(&n) == 0)      // nb of tiles of the sub-image
      status = FPX_FILE_WRITE_ERROR;
    if (subStreamHdr->WriteVT_I4(&tileWidth) == 0)  // Tiles width
      status = FPX_FILE_WRITE_ERROR;
    if (subStreamHdr->WriteVT_I4(&tileWidth) == 0)  // Tiles height (the same in Baseline...)
      status = FPX_FILE_WRITE_ERROR;
    if (subStreamHdr->WriteVT_I4(&nbChannel) == 0)  // Number of channels
      status = FPX_FILE_WRITE_ERROR;
    if (subStreamHdr->WriteVT_I4(&offset) == 0)   // Tile header table offset
      status = FPX_FILE_WRITE_ERROR;
    if (subStreamHdr->WriteVT_I4(&length) == 0)   // Tile header table offset
      status = FPX_FILE_WRITE_ERROR;
  
    // Write the Tile Header Table
    if (status == FPX_OK) {
    
      // For each tile
      for (long i = 0; i < (long) n; i++) {

        // Get the pointer for this tile
        PTileFlashPix* pt   = (PTileFlashPix*)(tiles + i);
        
        // If this tile has never been written before, initialize it with background color
        long offset = pt->GetposFic();
        if ( (offset < 0 ) && (pt->compression != SINGLE_COLOR)) {  // PTCH_NEGONE
          pt->tileInitialize = TRUE; 
          
          // Set the compression to single color and write tile
          pt->SetCompression(SINGLE_COLOR); 
          pt->WriteTile(); 
        }
        
        // Get the infos for this tile
        offset          = pt->GetposFic();
        DWORD size          = pt->GetTileSize();
        DWORD blocCompr     = (DWORD)pt->compression;
        DWORD blocSubtype   = (DWORD)pt->compressionSubtype;

        // Change compression type to 2 (JPEG) only 
        if ( pt->compression == JPEG_BY_QUALITY || pt->compression == JPEG_BY_TABLE_GROUP )
          blocCompr = JPEG_UNSPECIFIED; 
          
        // Write the Tile Header for this tile
        if (subStreamHdr->WriteVT_I4((DWORD *)&offset) == 0)// Write the offset of each tile of the sub-image in the file
          status = FPX_FILE_WRITE_ERROR;
        if (subStreamHdr->WriteVT_I4(&size) == 0)     // Write the size of each tile
          status = FPX_FILE_WRITE_ERROR;
        if (subStreamHdr->WriteVT_I4(&blocCompr) == 0)    // Write the compression type
          status = FPX_FILE_WRITE_ERROR;
        if (subStreamHdr->WriteVT_I4(&blocSubtype) == 0)  // Write the sub-compression type
          status = FPX_FILE_WRITE_ERROR;

        // Break and exit if error
        if (status != FPX_OK)
          break;
      }
    }
  }
  
  return (status);
}


//  ----------------------------------------------------------------------------
// Read the tile data from the file without decompression
FPXStatus PResolutionFlashPix::ReadRawTile (
          unsigned long          whichTile, 
          FPXCompressionOption*  compressOption, 
          unsigned char*         compressQuality,
          long*                  compressSubtype,
          unsigned long*         dataLength,
          void**                 data)
{
  FPXStatus status = FPX_OK;

  // Read and allocate tile pointers if necessary
  if (!HasBeenUsed()) {
    status = ReadHeaderStream();
    if(status != FPX_OK)
      return status;
  }

  if (status == 0) {
    unsigned long nbTiles = nbTilesW * nbTilesH;
    if (whichTile >= nbTiles)
      status = FPX_BAD_COORDINATES;
    else {
      PTileFlashPix* tile = (PTileFlashPix*)(tiles) + whichTile;
      status = tile->ReadRawTile (compressOption, compressQuality, compressSubtype, dataLength, data);
    }
  }
  return status;
}


//  ----------------------------------------------------------------------------
// Write the tile data in the file without compression
FPXStatus PResolutionFlashPix::WriteRawTile (
          unsigned long       whichTile, 
          FPXCompressionOption  compressOption, 
          unsigned char       compressQuality,
          long          compressSubtype,
          unsigned long       dataLength,
          void*           data)
{
  FPXStatus status = FPX_OK;

  // Read and allocate tile pointers if necessary
  if (!HasBeenUsed())
    status = UpdateHeaderStream();
  
  if (status == FPX_OK) {
    unsigned long nbTiles = nbTilesW * nbTilesH;
    if (whichTile >= nbTiles)
      status = FPX_BAD_COORDINATES;
    else {
      PTileFlashPix* tile = (PTileFlashPix*)(tiles) + whichTile;
      status = tile->WriteRawTile (compressOption, compressQuality, compressSubtype, dataLength, data);
    }
  }
  return status;
}


//  ----------------------------------------------------------------------------
FPXStatus PResolutionFlashPix::WriteLine (Pixel* pix, short plan)
{
  FPXStatus status = FPX_OK;
  
  // Read and allocate tile pointers if necessary
  if (!HasBeenUsed()) 
    status = UpdateHeaderStream();
  
  // Call the parent method now
  if (status == FPX_OK)
    status = PResolutionLevel::WriteLine(pix, plan);
    
  return status;
}


//  ----------------------------------------------------------------------------
FPXStatus PResolutionFlashPix::FlushModifiedTiles()
{
  return PResolutionLevel::FlushModifiedTiles() ;
}


//  ----------------------------------------------------------------------------
FPXStatus PResolutionFlashPix::Convolution (long x, long y, Pixel* pix, long width, long height)
{
  FPXStatus status = FPX_OK;
  
  // Read and allocate tile pointers if necessary
  if (!HasBeenUsed())
    status = UpdateHeaderStream();

  // Call the parent method now
  if (status == FPX_OK)
    status = PResolutionLevel::Convolution (x, y, pix, width, height);
    
  return status;
}


//  ----------------------------------------------------------------------------
FPXStatus PResolutionFlashPix::DecimateLevel ()
{
  FPXStatus status = FPX_OK;
  
  // Read and allocate tile pointers if necessary
  if (!HasBeenUsed())
      status = UpdateHeaderStream();
  
  // Call the parent method now
  if (status == FPX_OK)
    status = PResolutionLevel::DecimateLevel();
    
  return status;
}


//  ----------------------------------------------------------------------------
FPXStatus PResolutionFlashPix::WriteRectangle (long x0, long y0, long x1, long y1, Pixel* pix, short plan)
{
  FPXStatus status = FPX_OK;
  
  // Read and allocate tile pointers if necessary
  if (!HasBeenUsed())
    status = UpdateHeaderStream();
  
  // Call the parent method now
  if (status == FPX_OK)
    status = PResolutionLevel::WriteRectangle(x0, y0, x1, y1, pix, plan);
    
  return status;
}


//  ----------------------------------------------------------------------------
FPXStatus PResolutionFlashPix::ReadRectangle (long x0, long y0, long x1, long y1, Pixel* pix)
{
  FPXStatus status = FPX_OK;
  
  // Read and allocate tile pointers if necessary
  if (!HasBeenUsed()) 
    status = ReadHeaderStream();
  
  // Call the parent method now
  if (status == FPX_OK)
    status = PResolutionLevel::ReadRectangle(x0, y0, x1, y1, pix);
    
  return status;
}


//  ----------------------------------------------------------------------------
FPXStatus PResolutionFlashPix::ReadInARectangle (Pixel* bufferOut, short pixelsPerLine, short width, short height,
                     const CorrectLut* correctLut, Boolean useAlphaChannel, const CombinMat* combinaisonMatrix)
{
  FPXStatus status = FPX_OK;
  
  // Read and allocate tile pointers if necessary
  if (!HasBeenUsed())
    status = ReadHeaderStream();

  // Call the parent method now
  if (status == 0)
    status = PResolutionLevel::ReadInARectangle (bufferOut, pixelsPerLine, width, height,
                        correctLut, useAlphaChannel, combinaisonMatrix);
  return status;
}


//  ----------------------------------------------------------------------------
FPXStatus PResolutionFlashPix::Read (long* px, long* py, Pixel* table)
{
  FPXStatus status = FPX_OK;
  
  // Read and allocate tile pointers if necessary
  if (!HasBeenUsed())
    status = ReadHeaderStream();

  // Call the parent method now
  if (status == FPX_OK)
    status = PResolutionLevel::Read (px, py, table);

  return status;
}


//  ----------------------------------------------------------------------------
FPXStatus PResolutionFlashPix::ReadInterpolated (long* px, long* py, Pixel* table)
{
  FPXStatus status = FPX_OK;
  
  // Read and allocate tile pointers if necessary
  if (!HasBeenUsed())
    status = ReadHeaderStream();

  // Call the parent method now
  if (status == FPX_OK)
    status = PResolutionLevel::ReadInterpolated (px, py, table);

  return status;
}


//  ----------------------------------------------------------------------------
FPXStatus PResolutionFlashPix::ReadMeanInterpolated (long xi, long yi, Pixel& pixel)
{
  FPXStatus status = FPX_OK;
  
  // Read and allocate tile pointers if necessary
  if (!HasBeenUsed())
    status = ReadHeaderStream();

  // Call the parent method now
  if (status == FPX_OK)
    status = PResolutionLevel::ReadMeanInterpolated (xi, yi, pixel);

  return status;
}


//  ----------------------------------------------------------------------------
Boolean PResolutionFlashPix::IsOnTheBorder (long xi, long yi)
{
  Boolean ok = true;
  
  // Read and allocate tile pointers if necessary
  if (!HasBeenUsed()) {
    if (UpdateHeaderStream() != FPX_OK)
      ok = false;
  }

  // Call the parent method now
  if (ok)
    ok = PResolutionLevel::IsOnTheBorder (xi, yi);

  return ok;
}


//  ----------------------------------------------------------------------------
// Compute histogram for the 4 channels 
FPXStatus PResolutionFlashPix::GetHistogram (long* alpha, long* red, long* green, long* blue, long* brightness, const CorrectLut* correctLut)
{
  FPXStatus status = FPX_OK;
  
  // Read and allocate tile pointers if necessary
  if (!HasBeenUsed())
    status = UpdateHeaderStream();

  // Call the parent method now
  if (status == 0)
    status = PResolutionLevel::GetHistogram (alpha, red, green, blue, brightness, correctLut);

  return status;
}


//  ----------------------------------------------------------------------------
FPXStatus PResolutionFlashPix::GetResolutionSizeInfo (long* width, long* height, long* nbTilesWidth, long* nbTilesHeight)
{
  FPXStatus status = FPX_OK;
  
  // Read and allocate tile pointers if necessary
  if (!HasBeenUsed())
    status = UpdateHeaderStream();

  // Call the parent method now
  if (status == 0)
    status = PResolutionLevel::GetResolutionSizeInfo (width, height, nbTilesWidth, 
                      nbTilesHeight);

  return status;
}


//  ----------------------------------------------------------------------------
FPXStatus PResolutionFlashPix::ReadSampledRectangle(long x0, long y0,long x1, long y1, Pixel* map, short pixelsPerLine, long mapWidth, long mapHeight, Boolean showAlphaChannel, float ratio)
{
  FPXStatus status = FPX_OK;
  
  // Read and allocate tile pointers if necessary
  if (!HasBeenUsed()) 
    status = ReadHeaderStream();

  // Call the parent method now
  if (status == FPX_OK)
    status = PResolutionLevel::ReadSampledRectangle (x0, y0, x1, y1, map, pixelsPerLine, 
                      mapWidth, mapHeight, showAlphaChannel, ratio);
  return status;
}


//  ----------------------------------------------------------------------------
FPXStatus PResolutionFlashPix::SearchPixelTopLeftCorner(long* x1, long* y1, float ratio)
{
  FPXStatus status = FPX_OK;
  
  // Read and allocate tile pointers if necessary
  if (!HasBeenUsed())
    status = UpdateHeaderStream();

  // Call the parent method now
  if (status == FPX_OK)
    status = PResolutionLevel::SearchPixelTopLeftCorner (x1, y1, ratio);

  return status;
}


//  ----------------------------------------------------------------------------
// Read a pixel of a sub image. Read it on disk if necessary. 
// If error, return false and pixel is set to 0.
FPXStatus PResolutionFlashPix::ReadMean (long xi, long yi, Pixel& pixel)
{
  FPXStatus status = FPX_OK;
  
  // Read and allocate tile pointers if necessary
  if (!HasBeenUsed()) 
    status = ReadHeaderStream();

  // Call the parent method now
  if (status == FPX_OK)
    status = PResolutionLevel::ReadMean (xi, yi, pixel);

  return status;
}

//  ----------------------------------------------------------------------------
//  External Functions
//  ----------------------------------------------------------------------------



//  - EOF ----------------------------------------------------------------------
