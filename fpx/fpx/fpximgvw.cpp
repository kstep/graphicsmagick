//  ----------------------------------------------------------------------------
//  MODULE    : FPXImageView.cpp
//  LANGUAGE  : C++ 
//  CREATOR   : Yue Zhang 
//  DATE    : March 21, 1996 
//  DESCRIPTION :  
//  COMMENTS  : 
//      SCCSID          : @(#)fpximgvw.cpp  1.9 12:46:34 01 Jul 1997
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------

//  ----------------------------------------------------------------------------
  #include "fpximgvw.h"
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

#ifndef OLECommun_h
  #include "olecomm.h"
#endif

#ifndef OLECore_h
  #include "olecore.h"
#endif

#ifndef Gen_guid_h
  #include "gen_guid.h"
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

#ifndef ColorTwist_h
  #include "coltwist.h"
#endif

#ifndef FileFlashPixIO_h
  #include "f_fpxio.h"
#endif

#ifndef PageIVUE_h
  #include "ri_page.h"
#endif

#ifndef BufferDesc_h
  #include  "buffdesc.h"
#endif

#ifndef FPXThumbnail_h
  #include  "thmbnail.h"
#endif

//  Constants
//  ---------

//  Variables
//  ---------

//  ----------------------------------------------------------------------------
//  Internal Functions
//  ----------------------------------------------------------------------------

//  ----------------------------------------------------------------------------
//  Member Functions
//  ----------------------------------------------------------------------------

// Open the file in Read Mode only
// CHG_VIS_OUT - added visibleOutputIndex argument to be passed to PFileFlashPixView
PFlashPixImageView::PFlashPixImageView (FicNom& refName, const char* theStorageName, 
                    long visibleOutputIndex,
                    FPXStatus * fpxStatus) : ViewImage(refName)

{
  CLSID idBaseline     = ID_Baseline;
  CLSID idImageView    = ID_ImageView;
  CLSID id;
  
  // Init the file pointers
  filePtr = NULL;
  image   = NULL;

  internalBuffer = NULL;
  internalBufSize = 0;

  // Call the generic search function which may be trapped by the different toolkits
  refName.Search ();

  // Get the CLSID of the file
  OLEFile file (refName, theStorageName);

  // TKTK Even though new is not used to allocate "file", 
  // an OLEError could have occurred in the OLEFile constructor
  // The most prudent thing to do is to check for an OLEError NOW, and 
  // return if an error did occur.
  file.GetCLSID(&id);
  
  if (id == idBaseline) {
    // If this is a FPX Image object:
    filePtr = NULL;                           // There is no Image View info
    image   = new PFileFlashPixIO (refName, theStorageName, mode_Lecture);  // Just open directly the Image Object
  } else if (id == idImageView) {
    // If this is a FPX Image View object:
    char sourceImageName [33];
    filePtr = new PFileFlashPixView (refName, theStorageName, mode_Lecture,
              visibleOutputIndex);              // Open the Image View
    if (filePtr==NULL) {
      return;
    }
    // PTCH_OBJFIX - use the index of the Source Description object to form the
    //  name of the source storage.
    GetImageStoreName(sourceImageName, filePtr->GetSourceDescNumber());
    image = new PFileFlashPixIO(filePtr->GetRootStorage(), sourceImageName, mode_Lecture);  // Then open the source storage inside it
  }

  if (image == NULL)
    return;
    
  long status = ((PHierarchicalImage*)(image))->OpenImage();
  
  // Test if Image file is successfully opened
  if (status || image->Status() != 0) {
    delete image;
    image = NULL;
    return;
  }

  // Init Viewing parameters
  InitViewParameters();
  transformsHaveBeenEdited  = FALSE;
  imageHasBeenEdited      = FALSE;

  // Load the FPX Image View viewing parameters (must be done AFTER opening the image and
  // initializing the viewing parameters)
  OpenFile();
}


//  ----------------------------------------------------------------------------
// Open the file in Read Mode only
// CHG_VIS_OUT - added visibleOutputIndex argument to be passed to PFileFlashPixView
PFlashPixImageView::PFlashPixImageView (OLEStorage* theOwningStorage, const char* theStorageName,
                    long visibleOutputIndex) : ViewImage()
{
  CLSID idBaseline     = ID_Baseline;
  CLSID idImageView    = ID_ImageView;
  CLSID id;

  // Init the file pointers
  filePtr = NULL;
  image   = NULL;

  internalBuffer = NULL; 
  internalBufSize = 0; 

  // Get the CLSID of the storage
  {
  STATSTG stat;
  theOwningStorage->Stat(&stat);
  id = stat.clsid;
  }
  
  if (id == idBaseline) {
    // If this is a FPX Image object:
    filePtr = NULL;                                 // There is no Image View info
    image   = new PFileFlashPixIO (theOwningStorage, theStorageName, mode_Lecture); // Just open directly the Image Object
  } else if (id == idImageView) {
    // If this is a FPX Image View object:
    filePtr = new PFileFlashPixView (theOwningStorage, theStorageName, mode_Lecture,
                    visibleOutputIndex);              // Open the Image View
    if (filePtr==NULL) {
      return;
    }
    
    // PTCH_OBJFIX - use the index of the Source Description object to form the
    //  name of the source storage.
    char sourceImageName [33];
    GetImageStoreName(sourceImageName, filePtr->GetSourceDescNumber());
    image = new PFileFlashPixIO(filePtr->GetRootStorage(), sourceImageName, mode_Lecture);  // Then open the source storage inside it
  }

  if (image == NULL)
    return;
    
  long status = ((PHierarchicalImage*)(image))->OpenImage();
  
  // Test if Image file is successfully opened
  if (status || image->Status() != 0) {
    delete image;
    image = NULL;
    return;
  }
  
  // Init Viewing parameters
  InitViewParameters();
  transformsHaveBeenEdited  = FALSE;
  imageHasBeenEdited      = FALSE;

  // Load the FPX Image View viewing parameters (must be done AFTER opening the image and
  // initializing the viewing parameters)
  OpenFile();
}


//  ----------------------------------------------------------------------------
// Open the file in Create Mode 
PFlashPixImageView::PFlashPixImageView (FicNom& refName, long width, long height, float resolution,
                  FPXBaselineColorSpace baseSpace, unsigned long backColor, 
                  FPXCompressionOption FPXCompressOption, Boolean createFPXImageView, Boolean baseUncalibrated)
                  : ViewImage(refName)
{

  internalBuffer = NULL; 
  internalBufSize = 0; 

  if (!createFPXImageView) {
    filePtr = NULL;                             // There is no Image View info
    // Create directly the Image Object
    image   = new PFileFlashPixIO (refName, width, height, resolution, baseSpace, backColor, FPXCompressOption, baseUncalibrated);
  }
  else {
    // Create a FPX Image View object:
    char sourceImageName [33];
    filePtr = new PFileFlashPixView (refName, NULL, mode_Ecrasement);
    if (filePtr==NULL) {
      return;
    }
    GetImageStoreName(sourceImageName, 1);
    // Then create a FPX Image Object inside it
    image = new PFileFlashPixIO(filePtr->GetRootStorage(), sourceImageName, 
                 width, height, resolution, baseSpace, backColor, FPXCompressOption, baseUncalibrated);
  }
  
  if (image == NULL)
    return;
    
  long status = ((PHierarchicalImage*)(image))->OpenImage();
  
  // Test if the FPX Image Object has been successfully opened
  if (status || image->Status() != 0) {
    delete image;
    image = NULL;
    return;
  }
  
  // Init Viewing parameters
  InitViewParameters();
  transformsHaveBeenEdited  = FALSE;
  imageHasBeenEdited      = TRUE;

  // Normalize the FPX Image View viewing parameters (must be done AFTER opening the image and
  // initializing the viewing parameters)
  OpenFile();
}


//  ----------------------------------------------------------------------------
// Open the file in Create Mode 
PFlashPixImageView::PFlashPixImageView (OLEStorage* theOwningStorage, const char* theStorageName, long width, long height, float resolution,
                  FPXBaselineColorSpace baseSpace, unsigned long backColor, 
                  FPXCompressionOption FPXCompressOption, Boolean createFPXImageView, Boolean baseUncalibrated)
                  : ViewImage()
{
  internalBuffer = NULL; 
  internalBufSize = 0; 

  if (!createFPXImageView) {
    filePtr = NULL;                             // There is no Image View info
    // Create directly the Image Object
    image   = new PFileFlashPixIO (theOwningStorage, theStorageName, width, height, resolution, baseSpace, backColor, FPXCompressOption, baseUncalibrated);
  }
  else {
    // Create a FPX Image View object:
    char sourceImageName [33];
    filePtr = new PFileFlashPixView (theOwningStorage, theStorageName, mode_Ecrasement);
    if (filePtr==NULL) {
      return;
    }
    GetImageStoreName(sourceImageName, 1);
    
    // Then create a FPX Image Object inside it
    image = new PFileFlashPixIO(filePtr->GetRootStorage(), sourceImageName, 
                 width, height, resolution, baseSpace, backColor, FPXCompressOption, baseUncalibrated);
  }

  if (image == NULL)
    return;
    
  long status = ((PHierarchicalImage*)(image))->OpenImage();
  
  // Test if Image file is successfully opened
  if (status || image->Status() != 0) {
    delete image;
    image = NULL;
    return;
  }
  
  // Init Viewing parameters
  InitViewParameters();
  transformsHaveBeenEdited  = FALSE;
  imageHasBeenEdited      = TRUE;

  // Normalize the FPX Image View viewing parameters (must be done AFTER opening the image and
  // initializing the viewing parameters)
  OpenFile();
}


//  ----------------------------------------------------------------------------
// Open the file in Read/Write mode (but not Create)
// CHG_VIS_OUT - added visibleOutputIndex argument to be passed to PFileFlashPixView
PFlashPixImageView::PFlashPixImageView (FicNom& refName, const char* theStorageName, 
                                        mode_Ouverture openMode, long visibleOutputIndex, 
                                        PSearchHookObject* /*hook*/, FPXStatus *fpxStatus) 
  : ViewImage(refName),
    readOnlyFile(FALSE)
{
  CLSID idBaseline  = ID_Baseline;
  CLSID idImageView = ID_ImageView;
  CLSID id;

  internalBuffer = NULL; 
  internalBufSize = 0; 

  // Call the generic search function which may be trapped by the different toolkits
  refName.Search ();

  // Get the CLSID of the file
  OLEFile file (refName, theStorageName);
  
  file.GetCLSID(&id);
  *fpxStatus = file.getFPXStatus(); 
  if(*fpxStatus != FPX_OK)
    return;
  
  if(file.isFileReadOnly())
    readOnlyFile = TRUE;

  if (id == idBaseline) {
    // If this is a FPX Image object:
    filePtr = NULL;                         // There is no Image View info
    image   = new PFileFlashPixIO (refName, theStorageName, openMode);  // Just open directly the Image Object
  } else if (id == idImageView) {
    // If this is a FPX Image View object:
    char sourceImageName [33];
    filePtr = new PFileFlashPixView (refName, theStorageName, openMode,
                                     visibleOutputIndex);      // Open the Image View
    if (filePtr == NULL) {
      return;
    }
    // PTCH_OBJFIX - use the index of the Source Description object to form the
    //  name of the source storage.
    GetImageStoreName(sourceImageName, filePtr->GetSourceDescNumber());
    image = new PFileFlashPixIO(filePtr->GetRootStorage(), sourceImageName, openMode);  // Then open the source storage inside it
  }

  if (image == NULL)
    return;
    
  long status = ((PHierarchicalImage*)(image))->OpenImage();
  
  // Test if Image file is successfully opened
  if (status || image->Status() != 0) {
    delete image;
    image = NULL;
    return;
  }
  
  // Init Viewing parameters
  InitViewParameters();
  transformsHaveBeenEdited  = FALSE;
  imageHasBeenEdited      = FALSE;

  // Load the FPX Image View viewing parameters (must be done AFTER opening the image and
  // initializing the viewing parameters)
  OpenFile();
}


//  ----------------------------------------------------------------------------
// Open the file in Read/Write mode (but not Create)
//  Called in ByStorage method
// CHG_VIS_OUT - added visibleOutputIndex argument to be passed to PFileFlashPixView
PFlashPixImageView::PFlashPixImageView (OLEStorage* theOwningStorage, const char* theStorageName, 
                      mode_Ouverture openMode, long visibleOutputIndex,
                      FPXStatus * fpxStatus ) : ViewImage()

{
  CLSID idBaseline  = ID_Baseline;
  CLSID idImageView = ID_ImageView;
  CLSID id;

  internalBuffer = NULL; 
  internalBufSize = 0; 

  // Get the CLSID of the storage
  {
  STATSTG stat;
  theOwningStorage->Stat(&stat);
  id = stat.clsid;
  }
  
  if (id == idBaseline) {
    // If this is a FPX Image object:
    filePtr = NULL;                               // There is no Image View info
    image   = new PFileFlashPixIO (theOwningStorage, theStorageName, openMode); // Just open directly the Image Object
  } else if (id == idImageView) {
    // If this is a FPX Image View object:
    char sourceImageName [33];
    filePtr = new PFileFlashPixView (theOwningStorage, theStorageName, openMode,
                    visibleOutputIndex);            // Open the Image View
    if (filePtr==NULL) {
      return;
    }
    // PTCH_OBJFIX - use the index of the Source Description object to form the
    //  name of the source storage.
    GetImageStoreName(sourceImageName, filePtr->GetSourceDescNumber());
    image = new PFileFlashPixIO(filePtr->GetRootStorage(), sourceImageName, openMode);  // Then open the source storage inside it
  }

  if (image == NULL)
    return;
    
  long status = ((PHierarchicalImage*)(image))->OpenImage();
  
  // Test if Image file is successfully opened
  if (status || image->Status() != 0) {
    delete image;
    image = NULL;
    return;
  }
  
  // Init Viewing parameters
  InitViewParameters();
  transformsHaveBeenEdited  = FALSE;
  imageHasBeenEdited      = FALSE;

  // Load the FPX Image View viewing parameters (must be done AFTER opening the image and
  // initializing the viewing parameters)
  OpenFile();
}


//  ----------------------------------------------------------------------------
// Create a FPX Image View from an already opened image
PFlashPixImageView::PFlashPixImageView (PRIImage* theImage) : ViewImage(theImage)
{
  filePtr = NULL;
}


//  ----------------------------------------------------------------------------
PFlashPixImageView::~PFlashPixImageView ()
{
  // Write things in the FPX Image View file if necessary and close it
  if (transformsHaveBeenEdited || imageHasBeenEdited)
    CloseFile();
  
  // Delete Image if present: to be done BEFORE deleting the embedding file
  if (image && !doNotDelete) {
    delete image;
    image = NULL;   // Reinit to avoid crash destruction in ViewImage
  }
  
  // Close and delete the embedding file
  delete filePtr;
  filePtr = NULL;

  if(internalBuffer)
  {
    delete internalBuffer;
    internalBuffer = NULL;
  }
}

void PFlashPixImageView::setInternalBuffer(long width, long height)
{
  if(width * height * 4 > internalBufSize)
  {
    delete internalBuffer;
    internalBuffer = NULL;
    internalBuffer = new unsigned char[width * height * 4];
    internalBufSize = width*height*4;
  }
}

//  ----------------------------------------------------------------------------
FPXStatus PFlashPixImageView::OpenFile()
{
  FPXStatus status = FPX_OK;

  // Init the transform values to default
  InitTransformProperties();

  // Load all tranform values from the file
  LoadImageResultAspectRatio();
  LoadImageROI();
  LoadImageFilteringValue();
  LoadImageAffineMatrix();
  LoadImageColorTwistMatrix();
  LoadImageContrastAdjustment();

  // Renormalize the original image size to 1 mm in height
  normalizationRatio = originHeight;
  width  /= normalizationRatio;
  height /= normalizationRatio;
  originWidth  /= normalizationRatio;
  originHeight /= normalizationRatio;
  long pixWidth, pixHeight;
  image->GetTrueDimensions(&pixWidth,&pixHeight);
  float resolution = (float)(pixHeight) / (float)(1.0);
  image->SetImageSize(pixWidth,pixHeight,resolution);

  // Update the parent object geometric parameters: original crop
  SetImageCrop ( regionOfInterest.left, 
           regionOfInterest.top , 
           regionOfInterest.left + regionOfInterest.width , 
           regionOfInterest.top  + regionOfInterest.height);

  // Update the parent object geometric parameters: position matrix
  // The transformation is limited to the Baseline case, i.e., 2D affine transformation  (see Appendix A, par. A.3.2)
  TransfoPerspective normPos (affineMatrix.a11, affineMatrix.a21,   // 2D scale, rotation and skewing 
                  affineMatrix.a12, affineMatrix.a22,
                  affineMatrix.a14, affineMatrix.a24,   // Translation  
                  affineMatrix.a31, affineMatrix.a32);  // Perspective
  position = normPos;

  // Update the parent object geometric parameters: final crop
  finalWidth  = aspectRatio;
  finalHeight = (float)1.0;
  resizeFinalDimensions = FALSE;

  // Update the parent object display parameters:
  SetFiltering (filtering);
  SetContrast (contrastAdjustment);

  
  PColorTwist twistMatrix (colorTwistMatrix);
  SetColorTwist (&twistMatrix);
  
  return status;
}


//  ----------------------------------------------------------------------------
FPXStatus PFlashPixImageView::CloseFile()
{
  FPXStatus status = FPX_OK;
  
  if(hasAffineMatrix)
  {
    LoadImageAffineMatrix();

    // Update the parent object geometric parameters: position matrix
    // The transformation is limited to the Baseline case, i.e., 2D affine transformation  (see Appendix A, par. A.3.2)
    TransfoPerspective normPos (affineMatrix.a11, affineMatrix.a21,   // 2D scale, rotation and skewing 
                  affineMatrix.a12, affineMatrix.a22,
                  affineMatrix.a14, affineMatrix.a24,   // Translation  
                  affineMatrix.a31, affineMatrix.a32);
    
    
    TransfoPerspective translat ((float)1.0, (float)0.0,(float) 0.0,(float) 1.0, -regionOfInterest.left, -regionOfInterest.top);

    position = normPos * translat;

    // Update the parent object geometric parameters: final crop
    finalWidth  = aspectRatio;
    finalHeight = (float)1.0;
    resizeFinalDimensions = FALSE;
  }
  if(hasFiltering)
  {
    LoadImageFilteringValue();
    SetFiltering(filtering);
  }
  if(hasColorTwistMatrix)
  {
    LoadImageColorTwistMatrix();
    PColorTwist twistMatrix (colorTwistMatrix);
    SetColorTwist (&twistMatrix);
  }

  if(hasContrastAdjustment)
  {
    LoadImageContrastAdjustment();
    SetContrast(contrastAdjustment);
  }
  // Update the geometric parameters using the parent object: final crop
  aspectRatio =  finalWidth / finalHeight;

  // Update the geometric parameters using the parent object: original crop
  regionOfInterest.left   = cropX0;
  regionOfInterest.top    = cropY0;
  regionOfInterest.width  = cropX1 - cropX0;
  regionOfInterest.height = cropY1 - cropY0;

  // Update the geometric parameters using the parent object: position matrix
  // The transformation is limited to the Baseline case, i.e., 2D affine transformation  (see Appendix A, par. A.3.2)
  TransfoPerspective translat ((float)1.0,(float) 0.0,(float)0.0,(float) 1.0, regionOfInterest.left, regionOfInterest.top);

  TransfoPerspective normPos = position * translat;

  float a, b, c, d, x0, y0, px, py;
  normPos.GetAffineMatrix(&a,&b,&c,&d,&x0,&y0);
  normPos.GetPerspective(&px,&py);
  affineMatrix.a11 = a; affineMatrix.a12 = c; affineMatrix.a13 = (float)0.0; affineMatrix.a14 = x0; 
  affineMatrix.a21 = b; affineMatrix.a22 = d; affineMatrix.a23 = (float)0.0; affineMatrix.a24 = y0;
  affineMatrix.a31 = px; 
  affineMatrix.a32 = py; 
  affineMatrix.a33 = (float)1.0; affineMatrix.a34 = (float)0.0; 
  affineMatrix.a41 = (float)0.0; affineMatrix.a42 = (float)0.0; affineMatrix.a43 = (float)0.0; affineMatrix.a44 = (float)1.0; 

  // Update the display parameters using the parent object:
  PColorTwist twistMatrix;
  GetFiltering (&filtering);
  GetContrast (&contrastAdjustment);
  GetColorTwist (&twistMatrix);
  twistMatrix.ConvertToFPX(&colorTwistMatrix);
  
  // Save the Property Sets: this is a straighforward implementation of the default
  // baseline behavior as specified in the spec.
  
  if(transformsHaveBeenEdited  || imageHasBeenEdited) 
  {
    if (status == FPX_OK)
      status = SaveSummaryInfoPropertySet();
    if (status == FPX_OK)
      status = SaveGlobalInfoPropertySet();
    if (status == FPX_OK)
      status = SaveSourcePropertySet();
  }

  if ( (hasRegionOfInterest || hasAffineMatrix || hasAspectRatio  || 
        hasFiltering || hasColorTwistMatrix || hasContrastAdjustment) && transformsHaveBeenEdited)
  {
    // The Result Property Set is created only if there is a viewing transform
    if (status == FPX_OK)
      status = SaveResultPropertySet();

    // The Transform Property Set is created only if there is a viewing transform
    if (status == FPX_OK)
      status = SaveTransformPropertySet();
    if (status == FPX_OK)
      status = SaveImageROI();
    if (status == FPX_OK)
      status = SaveImageResultAspectRatio();

    // The Operation Property Set is created only if there is a viewing transform
    if (status == FPX_OK)
      status = SaveOperationPropertySet();
  }

  if (status == FPX_OK)
    status = SaveExtensionListPropertySet();

  return status;
}


//  ------------------------------------------------------------------------------------
//  Default Baseline Behavior for FPX Image View Properties Access
//  There's no interface provided to the user to update these property sets. When saving
//  them, all SaveXxPropertySet() do the same:
//    - declare a record to hold the values
//    - init this record with default values
//    - read the values from the file
//    - update the values if necessary
//    - write back the values in the file
//  ------------------------------------------------------------------------------------
FPXStatus PFlashPixImageView::SaveSummaryInfoPropertySet()
{
  FPXStatus status = FPX_OK;

  OLEProperty* aProp;
  OLEProperty* aProp0;
  
  
  // Create a thumbnail image in summary info property set if it doesn't exist
  if ( filePtr ) {
    if ( !filePtr->GetSummaryInfoProperty (PID_THUMBNAIL, &aProp0) ) {
      if ( filePtr->SetSummaryInfoProperty (PID_THUMBNAIL, TYP_THUMBNAIL, &aProp) ) {
        status = MakeNewThumbnail ( aProp);
      } else 
        status = FPX_NOT_A_VIEW;

    // Else if image has been changed, write the thumbnail image again
    } else if ( imageHasBeenEdited || transformsHaveBeenEdited ) {
    
      if ( filePtr->SetSummaryInfoProperty (PID_THUMBNAIL, TYP_THUMBNAIL, &aProp) ) {
        status = MakeNewThumbnail ( aProp);
      } else 
        status = FPX_NOT_A_VIEW;
    } else 
      status = FPX_NOT_A_VIEW;
  } else 
    status = FPX_NOT_A_VIEW;
  return status;  
}


//  ----------------------------------------------------------------------------
// Create a new thumbnail image and save it in the file
FPXStatus PFlashPixImageView::MakeNewThumbnail ( OLEProperty *aProp)
{
  FPXStatus status = FPX_OK;

  // Determine the width and height of thumbnail
  long  thumbNailWidth, thumbNailHeight; 
  if ( finalWidth >= finalHeight ) {
    thumbNailWidth = MaxThumbNailSize;
    thumbNailHeight = (long)((float)( thumbNailWidth * finalHeight ) / (float)finalWidth); 
  }
  else {
    thumbNailHeight = MaxThumbNailSize; 
    thumbNailWidth = (long)((float)( thumbNailHeight * finalWidth ) / (float)finalHeight); 
  }

  // The thumbnail should either be monochrome or 24-bit RGB (never YCC). Save the old
  //  image's colorSpace and force a new one specifically for the thumbnail
  // CHG_MONO_TINT - since a mono image is affected by colorTwist, don't make a
  //  monochrome thumbnail if there is a colorTwist defined.
  FPXBaselineColorSpace imageColorSpace = ((PFileFlashPixIO *)image)->baseSpace,
              tnailColorSpace;
  if (((imageColorSpace == SPACE_32_BITS_MA) || (imageColorSpace == SPACE_32_BITS_AM)
   ||  (imageColorSpace == SPACE_32_BITS_M)) && (hasColorTwist == false) )
    tnailColorSpace = SPACE_32_BITS_M;
  else
    tnailColorSpace = SPACE_32_BITS_RGB;
  ((PFileFlashPixIO *)image)->SetUsedColorSpace( tnailColorSpace);

  // Create a buffer to hold the image to be created
  FPXBufferDesc thumbNail( (long)1, thumbNailWidth, thumbNailHeight, tnailColorSpace);
  if (thumbNail.Get32BitsBuffer() == NULL) {
      return FPX_MEMORY_ALLOCATION_FAILED;
  }
  
  // Thumbnails are to be composited on a white background. Save the old background
  //  color and set the current background color to white.
  Pixel         backgroundColor = GtheSystemToolkit->backgroundColor;
  Pixel         whiteColor    = 0xFFFFFFFF;
  GtheSystemToolkit->SetBackgroundColor (whiteColor, tnailColorSpace);
    
  // Now render a new thumbnail image
  PageImage page (this, thumbNailWidth, thumbNailHeight, (float)0.0);
  page.ReadPage(thumbNail.Get32BitsBuffer());
  
  // Create the thumbnail property in the CLIPDATA format
  CLIPDATA  cf; 
  if ( CreateThumbnail((unsigned char *)thumbNail.Get32BitsBuffer(), tnailColorSpace, 
              (short)thumbNailWidth, (short)thumbNailHeight,&cf) )  {
    *aProp = (CLIPDATA *)&cf;
    delete cf.pClipData; 
  }
  else 
    status = FPX_ERROR;
  
  // Reset the background color and colorSpace
  GtheSystemToolkit->SetBackgroundColor (backgroundColor, imageColorSpace);
  ((PFileFlashPixIO *)image)->SetUsedColorSpace( imageColorSpace);

  return status;
}


//  ----------------------------------------------------------------------------
FPXStatus PFlashPixImageView::SaveGlobalInfoPropertySet()
{
  FPXStatus status = FPX_OK;

  // Declare a local record and initialize it with default values
  FPXGlobalInfo globalInfo;
  InitGlobalInfoPropertySet (&globalInfo);

  // Read the values already written in the file
  status = GetGlobalInfoPropertySet (&globalInfo);

  if ( (hasRegionOfInterest || hasAffineMatrix    || hasAspectRatio       || 
    hasFiltering    || hasColorTwistMatrix  || hasContrastAdjustment)
    && transformsHaveBeenEdited)
  {

    *(globalInfo.visibleOutputs.ptr)  = 2;
    // Maximum index used by default
    globalInfo.maxImageIndex     = 2; // 2 images (source and result)
    globalInfo.maxTransformIndex = 1; // 1 transformation
    globalInfo.maxOperatorIndex  = 1; // 1 operation
  }
  // Update with specific information
  if (status == FPX_OK && (imageHasBeenEdited || transformsHaveBeenEdited)) {
    if (globalInfo.lastModifierIsValid) {
      FPX_DeleteFPXWideStr(&(globalInfo.lastModifier));
      // This string has to be customized by the user in some ways...
      FPX_WideStrcpy(&(globalInfo.lastModifier),"Reference Implementation");
    }
  }
  // Write the values back in the file  
  if (status == FPX_OK)
    status = SetGlobalInfoPropertySet (&globalInfo);
    
  return status;

}


//  ----------------------------------------------------------------------------
FPXStatus PFlashPixImageView::SaveSourcePropertySet()
{
  FPXStatus   status = FPX_OK;

  // Declare a local record and initialize it with default values
  FPXImageDescription imageDesc;
  InitSourcePropertySet (&imageDesc);

  // Read the values already written in the file
  status = GetSourcePropertySet (&imageDesc);
  
  // CHG_WRT_USERS - If there is a result property set, then a 'users' array must be
  //      written. So test for a result property set and write 'users' if need be.
  OLEProperty     *aProp;
  long        hasResultPropSet = 0;

  if (filePtr)
    if (filePtr->GetResultDescProperty (PID_ImageID, &aProp))
      hasResultPropSet = 1;
  
  if((hasRegionOfInterest   || hasAffineMatrix    || hasAspectRatio       || 
    hasFiltering      || hasColorTwistMatrix  || hasContrastAdjustment  ||
    hasResultPropSet))
  {
    if(!imageDesc.users.ptr)
      FPX_AllocFPXLongArray(&(imageDesc.users) ,1);
    *(imageDesc.users.ptr)  = 1;
  }
  else
    // users should be 0 element array
    imageDesc.users.length  = 0;

  // Update the informations if the transformations have been modified
  if (status == FPX_OK && imageHasBeenEdited) {
    if (imageDesc.lastModifierIsValid) {
      FPX_DeleteFPXWideStr(&(imageDesc.lastModifier));
      // This string has to be customized by the user in some ways...
      FPX_WideStrcpy(&(imageDesc.lastModifier),"Reference Implementation");
    }
    if (imageDesc.revisionNumberIsValid)
      imageDesc.revisionNumber += 1;

    if (imageDesc.modificationDateIsValid)
      FPXUpdateTime(&(imageDesc.modificationDate));

    if (imageDesc.creatingApplicationIsValid) {
      FPX_DeleteFPXWideStr(&(imageDesc.lastModifier));

      // This string has to be customized by the user in some ways...
      FPX_WideStrcpy(&(imageDesc.creatingApplication),"Reference Implementation");
    }
  } 
  
  // PTCH_CACHEIMGSIZ - the size of the cached source image is required. If the size
  //  has not been set, then do so before saving.
  if ((imageDesc.heightIsValid == FALSE) || (imageDesc.widthIsValid == FALSE)) {
    long  w = 0;
    long  h = 0;
    if (image)
      image->GetDimensions (&w, &h);
    imageDesc.height = h;
    imageDesc.width  = w;
    imageDesc.heightIsValid = TRUE;
    imageDesc.widthIsValid  = TRUE;
  }
  
  // Write the values back in the file  
  if (status == FPX_OK)
    status = SetSourcePropertySet (&imageDesc);

  // Release the memory allocated (if any...)
  FPX_DeleteFPXLongArray (&(imageDesc.lockedProperties));
  FPX_DeleteFPXWideStr   (&(imageDesc.dataObjectTitle));
  FPX_DeleteFPXWideStr   (&(imageDesc.lastModifier));
  FPX_DeleteFPXWideStr   (&(imageDesc.creatingApplication));
  FPX_DeleteFPXLongArray (&(imageDesc.users));
  
  return status;
}



//  ----------------------------------------------------------------------------
FPXStatus PFlashPixImageView::SaveResultPropertySet()
{
  FPXStatus status = FPX_OK;

  // Declare a local record and initialize it with default values
  FPXImageDescription imageDesc;
  InitResultPropertySet (&imageDesc);

  // Read the values already written in the file
  status = GetResultPropertySet (&imageDesc);

  // Nothing to update in the result (the Reference Implementation never update the result)

  // Write the values back in the file  
  if (status == FPX_OK)
    status = SetResultPropertySet (&imageDesc); 

  // Release the memory allocated (if any...)
  FPX_DeleteFPXLongArray (&(imageDesc.lockedProperties));
  FPX_DeleteFPXWideStr   (&(imageDesc.dataObjectTitle));
  FPX_DeleteFPXWideStr   (&(imageDesc.lastModifier));
  FPX_DeleteFPXWideStr   (&(imageDesc.creatingApplication));
  FPX_DeleteFPXLongArray (&(imageDesc.users));

  return status;
}


//  ----------------------------------------------------------------------------
FPXStatus PFlashPixImageView::SaveTransformPropertySet()
{
  FPXStatus status = FPX_OK;

  // Declare a local record and initialize it with default values
  FPXTransform transform;
  InitTransformPropertySet (&transform);

  // Read the values already written in the file
  status = GetTransformPropertySet (&transform);

  // Update the informations if the transformations have been modified
  if (status == FPX_OK && transformsHaveBeenEdited) {
    if (transform.lastModifierIsValid) {
      FPX_DeleteFPXWideStr(&(transform.lastModifier));

      // This string has to be customized by the user in some ways...
      FPX_WideStrcpy(&(transform.lastModifier),"Reference Implementation");
    }
    if (transform.revisionNumberIsValid)
      transform.revisionNumber += 1;
    if (transform.modificationDateIsValid)
      FPXUpdateTime(&(transform.modificationDate));

    if (transform.creatingApplicationIsValid) {
      FPX_DeleteFPXWideStr(&(transform.lastModifier));
      // This string has to be customized by the user in some ways...
      FPX_WideStrcpy(&(transform.creatingApplication),"Reference Implementation");
    }
  }
  // Write the values back in the file  
  if (status == FPX_OK)
    status = SetTransformPropertySet (&transform);
  // Release the memory allocated (if any...)
  FPX_DeleteFPXLongArray (&(transform.lockedProperties));
  FPX_DeleteFPXWideStr   (&(transform.transformTitle));
  FPX_DeleteFPXWideStr   (&(transform.lastModifier));
  FPX_DeleteFPXWideStr   (&(transform.creatingApplication));
  FPX_DeleteFPXLongArray (&(transform.inputDataObjects));
  FPX_DeleteFPXLongArray (&(transform.outputDataObjects));
  
  return status;
}


//  ----------------------------------------------------------------------------
FPXStatus PFlashPixImageView::SaveOperationPropertySet()
{
  FPXStatus status = FPX_OK;

  // Declare a local record and initialize it with default values
  FPXOperation operation;
  InitOperationPropertySet (&operation);

  // Read the values already written in the file
  status = GetOperationPropertySet (&operation);

  // There is absolutly nothing to be updated here in Baseline.

  // Write the values back in the file  
  if (status == FPX_OK)
    status = SetOperationPropertySet (&operation);

  return status;
}


//  ----------------------------------------------------------------------------
FPXStatus PFlashPixImageView::SaveExtensionListPropertySet()
{
  return FPX_OK;
}


//  ----------------------------------------------------------------------------
void PFlashPixImageView::InitGlobalInfoPropertySet(FPXGlobalInfo* theGlobalInfo)
{
  // None of the optionnal property is made mandatory in the Reference Implementation
  theGlobalInfo->lockedPropertiesIsValid    = FALSE;
  theGlobalInfo->transformedImageTitleIsValid = FALSE;
  theGlobalInfo->lastModifierIsValid      = FALSE;
  
  // Initialize the arrays
  InitFPXLongArray (&(theGlobalInfo->lockedProperties));
  InitFPXWideStr   (&(theGlobalInfo->transformedImageTitle));
  InitFPXWideStr   (&(theGlobalInfo->lastModifier));

  // Default is the number of the source image
  FPX_AllocFPXLongArray(&(theGlobalInfo->visibleOutputs) ,1);
  *(theGlobalInfo->visibleOutputs.ptr)  = 1;
  
  // Maximum index used by default
  theGlobalInfo->maxImageIndex     = 1; // 2 images (source and result)
  theGlobalInfo->maxTransformIndex = 0; // 1 transformation
  theGlobalInfo->maxOperatorIndex  = 0; // 1 operation
}


//  ----------------------------------------------------------------------------
void PFlashPixImageView::InitSourcePropertySet(FPXImageDescription* theImageDesc)
{
  CLSID clsImageDesID;          // Generate a unique ID (what for?)
  GenGuid(&clsImageDesID);
  theImageDesc->dataObjectID  = clsImageDesID;

  // None of the optionnal property is made mandatory in the Reference Implementation
  theImageDesc->lockedPropertiesIsValid     = FALSE;
  theImageDesc->dataObjectTitleIsValid    = FALSE;
  theImageDesc->lastModifierIsValid       = FALSE;
  theImageDesc->revisionNumberIsValid     = FALSE;
  theImageDesc->creationDateIsValid       = FALSE;
  theImageDesc->modificationDateIsValid     = FALSE;
  theImageDesc->creatingApplicationIsValid  = FALSE;
  
  // Initialize the arrays
  InitFPXLongArray (&(theImageDesc->lockedProperties));
  InitFPXWideStr   (&(theImageDesc->dataObjectTitle));
  InitFPXWideStr   (&(theImageDesc->lastModifier));
  InitFPXWideStr   (&(theImageDesc->creatingApplication));

  // The default status is no cached result (useless...), so:
  // The source image is there and you cannot purge it...
  theImageDesc->status.existenceData    = FPX_DATA_CACHED;
  theImageDesc->status.permission     = FPX_DATA_NOT_PURGEABLE;

  // Always 0, according to the spec (see par. VIII.1.1)
  theImageDesc->creator = 0;

  // Must be the same as the operationNumber in the default transform property set
  FPX_AllocFPXLongArray(&(theImageDesc->users) ,1);
  *(theImageDesc->users.ptr)  = 1;
  
  // Get the pixels dimensions of the source image
  theImageDesc->heightIsValid = TRUE;
  theImageDesc->widthIsValid  = TRUE;
  long w = 0;
  long h = 0;
  if (image)
    image->GetDimensions (&w, &h);
  theImageDesc->height = h;
  theImageDesc->width  = w;
}


//  ----------------------------------------------------------------------------
void PFlashPixImageView::InitResultPropertySet(FPXImageDescription* theImageDesc)
{
  CLSID clsImageDesID;          // Generate a unique ID (what for?)
  GenGuid(&clsImageDesID);
  theImageDesc->dataObjectID  = clsImageDesID;

  // None of the optionnal property is made mandatory in the Reference Implementation
  theImageDesc->lockedPropertiesIsValid     = FALSE;
  theImageDesc->dataObjectTitleIsValid    = FALSE;
  theImageDesc->lastModifierIsValid       = FALSE;
  theImageDesc->revisionNumberIsValid     = FALSE;
  theImageDesc->creationDateIsValid       = FALSE;
  theImageDesc->modificationDateIsValid     = FALSE;
  theImageDesc->creatingApplicationIsValid  = FALSE;
  
  // Initialize the arrays
  InitFPXLongArray (&(theImageDesc->lockedProperties));
  InitFPXWideStr   (&(theImageDesc->dataObjectTitle));
  InitFPXWideStr   (&(theImageDesc->lastModifier));
  InitFPXWideStr   (&(theImageDesc->creatingApplication));

  // The default status is no cached result (useless...), so:
  // The result image is not there and you can purge it (well... sort of... it's not there...)
  theImageDesc->status.existenceData    = FPX_DATA_NOT_CACHED;
  theImageDesc->status.permission     = FPX_DATA_PURGEABLE;

  // Must the same as the operationNumber in the default transform property set
  theImageDesc->creator = 1;

  // No user for the result
  InitFPXLongArray (&(theImageDesc->users));

  // No cached image so no size 
  theImageDesc->heightIsValid = FALSE;
  theImageDesc->widthIsValid  = FALSE;
  theImageDesc->height    = 0;
  theImageDesc->width     = 0;
}


//  ----------------------------------------------------------------------------
void PFlashPixImageView::InitTransformPropertySet(FPXTransform* theTransform)
{
  CLSID clsTransformID;               // Generate a unique ID (what for?)
  GenGuid(&clsTransformID);
  CLSID clsOperationID = ID_Transform;        // Always this value in Baseline
  theTransform->transformNodeID  = clsTransformID;
  theTransform->operationClassID = clsOperationID;

  // None of the optionnal property is made mandatory in the Reference Implementation
  theTransform->lockedPropertiesIsValid     = FALSE;
  theTransform->transformTitleIsValid     = FALSE;
  theTransform->lastModifierIsValid       = FALSE;
  theTransform->revisionNumberIsValid     = FALSE;
  theTransform->creationDateIsValid       = FALSE;
  theTransform->modificationDateIsValid     = FALSE;
  theTransform->creatingApplicationIsValid  = FALSE;

  // Initialize the arrays
  InitFPXLongArray (&(theTransform->lockedProperties));
  InitFPXWideStr   (&(theTransform->transformTitle));
  InitFPXWideStr   (&(theTransform->lastModifier));
  InitFPXWideStr   (&(theTransform->creatingApplication));

  // Use the default numbers (see spec VIII.2.1)
  FPX_AllocFPXLongArray(&(theTransform->inputDataObjects) ,1);
  FPX_AllocFPXLongArray(&(theTransform->outputDataObjects),1);
  *(theTransform->inputDataObjects.ptr) = 1;
  *(theTransform->outputDataObjects.ptr)  = 2;
  theTransform->operationNumber     = 1;
}


//  ----------------------------------------------------------------------------
void PFlashPixImageView::InitOperationPropertySet(FPXOperation* theOperation)
{
  CLSID clsID = ID_Transform;       // Always this value in Baseline
  theOperation->operationID = clsID;
}


//  ----------------------------------------------------------------------------
void PFlashPixImageView::InitExtensionListPropertySet(FPXExtensionDescription* /*theExtension*/)
{
  // Not implemented yet...
}


//  ------------------------------------------------------------------------------------
//  Basic FPX Image View Properties Access
//  Set and Get property sets functions: one day, these functions will be made public and
//  provided to the user through some calls in the Reference Implementation API...
//  ------------------------------------------------------------------------------------
FPXStatus PFlashPixImageView::SetGlobalInfoPropertySet(FPXGlobalInfo* theGlobalInfo)
{
  FPXStatus err = FPX_OK;

  if (filePtr) {
    OLEProperty* aProp;
    
    if (theGlobalInfo->lockedPropertiesIsValid)
      if (filePtr->SetGlobalInfoProperty (PID_LockedPropertyList, TYP_LockedPropertyList, &aProp))
        *aProp = (FPXLongArray)(theGlobalInfo->lockedProperties);

    if (theGlobalInfo->transformedImageTitleIsValid)
      if (filePtr->SetGlobalInfoProperty (PID_Title, TYP_Title, &aProp))
        *aProp = (FPXWideStr)(theGlobalInfo->transformedImageTitle);

    if (theGlobalInfo->lastModifierIsValid)
      if (filePtr->SetGlobalInfoProperty (PID_LastModifier, TYP_LastModifier, &aProp))
        *aProp = (FPXWideStr)(theGlobalInfo->lastModifier);

    if (filePtr->SetGlobalInfoProperty (PID_VisibleOutputs, TYP_VisibleOutputs, &aProp))
      *aProp = (FPXLongArray)(theGlobalInfo->visibleOutputs);

    if (filePtr->SetGlobalInfoProperty (PID_MaxImageIndex, TYP_MaxImageIndex, &aProp))
      *aProp = (long)(theGlobalInfo->maxImageIndex);

    if (filePtr->SetGlobalInfoProperty (PID_MaxTransfoIndex, TYP_MaxTransfoIndex, &aProp))
      *aProp = (long)(theGlobalInfo->maxTransformIndex);

    if (filePtr->SetGlobalInfoProperty (PID_MaxOperatorIndex, TYP_MaxOperatorIndex, &aProp))
      *aProp = (long)(theGlobalInfo->maxOperatorIndex);

    filePtr->Commit();
  } else
    err = FPX_NOT_A_VIEW;
    
  return err;
}


//  ----------------------------------------------------------------------------
FPXStatus PFlashPixImageView::GetGlobalInfoPropertySet(FPXGlobalInfo* theGlobalInfo)
{
  FPXStatus err = FPX_OK;

  if (filePtr) {
    OLEProperty* aProp;
    
    if (filePtr->GetGlobalInfoProperty (PID_LockedPropertyList, &aProp)) {
      theGlobalInfo->lockedProperties = (FPXLongArray)(*aProp);
      theGlobalInfo->lockedPropertiesIsValid = TRUE;
    } else
      theGlobalInfo->lockedPropertiesIsValid = FALSE;

    if (filePtr->GetGlobalInfoProperty (PID_Title, &aProp)) {
      theGlobalInfo->transformedImageTitle = (FPXWideStr)(*aProp);
      theGlobalInfo->transformedImageTitleIsValid = TRUE;
    } else
      theGlobalInfo->transformedImageTitleIsValid = FALSE;

    if (filePtr->GetGlobalInfoProperty (PID_LastModifier, &aProp)) {
      theGlobalInfo->lastModifier = (FPXWideStr)(*aProp);
      theGlobalInfo->lastModifierIsValid = TRUE;
    } else
      theGlobalInfo->lastModifierIsValid = FALSE;

    if (filePtr->GetGlobalInfoProperty (PID_VisibleOutputs, &aProp))
      theGlobalInfo->visibleOutputs = (FPXLongArray)(*aProp);

    if (filePtr->GetGlobalInfoProperty (PID_MaxImageIndex, &aProp))
      theGlobalInfo->maxImageIndex = (long)(*aProp);

    if (filePtr->GetGlobalInfoProperty (PID_MaxTransfoIndex, &aProp))
      theGlobalInfo->maxTransformIndex = (long)(*aProp);

    if (filePtr->GetGlobalInfoProperty (PID_MaxOperatorIndex, &aProp))
      theGlobalInfo->maxOperatorIndex = (long)(*aProp);
  } else
    err = FPX_NOT_A_VIEW;
  
  return err;
}


//  ----------------------------------------------------------------------------
FPXStatus PFlashPixImageView::SetSourcePropertySet(FPXImageDescription* theSourceDescription)
{
  FPXStatus err = FPX_OK;

  if (filePtr) {
    OLEProperty* aProp;
    
    if (filePtr->SetSourceDescProperty (PID_ImageID, TYP_ImageID, &aProp))
      *aProp = (CLSID *)&(theSourceDescription->dataObjectID);

    if (theSourceDescription->lockedPropertiesIsValid)
      if (filePtr->SetSourceDescProperty (PID_LockedPropertyList, TYP_LockedPropertyList, &aProp))
        *aProp = (FPXLongArray)(theSourceDescription->lockedProperties);

    if (theSourceDescription->dataObjectTitleIsValid)
      if (filePtr->SetSourceDescProperty (PID_Title, TYP_Title, &aProp))
        *aProp = (FPXWideStr)(theSourceDescription->dataObjectTitle);

    if (theSourceDescription->lastModifierIsValid)
      if (filePtr->SetSourceDescProperty (PID_LastModifier, TYP_LastModifier, &aProp))
        *aProp = (FPXWideStr)(theSourceDescription->lastModifier);

    if (theSourceDescription->revisionNumberIsValid)
      if (filePtr->SetSourceDescProperty (PID_RevisionNumber, TYP_RevisionNumber, &aProp))
        *aProp = (long)(theSourceDescription->revisionNumber);

    if (theSourceDescription->creationDateIsValid)
      if (filePtr->SetSourceDescProperty (PID_CreationTime, TYP_CreationTime, &aProp))
        *aProp = *(FILETIME *)&(theSourceDescription->creationDate);

    if (theSourceDescription->modificationDateIsValid)
      if (filePtr->SetSourceDescProperty (PID_ModificationTime, TYP_ModificationTime, &aProp))
        *aProp = *(FILETIME *)&(theSourceDescription->modificationDate);

    if (theSourceDescription->creatingApplicationIsValid)
      if (filePtr->SetSourceDescProperty (PID_CreatingApplication, TYP_CreatingApplication, &aProp))
        *aProp = (FPXWideStr)(theSourceDescription->creatingApplication);

    // PLEASE WINDOWS TESTER: the following line may be little endian/big endian dependent...
    long longStatus = (theSourceDescription->status.existenceData << 16) | theSourceDescription->status.permission;
    if (filePtr->SetSourceDescProperty (PID_Status, TYP_Status, &aProp))
      *aProp = (long)(longStatus);

    if (filePtr->SetSourceDescProperty (PID_Creator, TYP_Creator, &aProp))
      *aProp = (long)(theSourceDescription->creator);


    if (filePtr->SetSourceDescProperty (PID_Users, TYP_Users, &aProp))
      *aProp = (FPXLongArray)(theSourceDescription->users);

    if (theSourceDescription->heightIsValid)
      if (filePtr->SetSourceDescProperty (PID_CachedImageHeight, TYP_CachedImageHeight, &aProp))
        *aProp = (long)(theSourceDescription->height);

    if (theSourceDescription->widthIsValid)
      if (filePtr->SetSourceDescProperty (PID_CachedImageWidth, TYP_CachedImageWidth, &aProp))
        *aProp = (long)(theSourceDescription->width);

    filePtr->Commit();
  } else

    err = FPX_NOT_A_VIEW;

  return err;
}


//  ----------------------------------------------------------------------------
FPXStatus PFlashPixImageView::GetSourcePropertySet(FPXImageDescription* theSourceDescription)
{
  FPXStatus err = FPX_OK;

  if (filePtr) {
    OLEProperty* aProp;
    
    if (filePtr->GetSourceDescProperty (PID_ImageID, &aProp))
      theSourceDescription->dataObjectID = *(CLSID*)(*aProp);

    if (filePtr->GetSourceDescProperty (PID_LockedPropertyList, &aProp)) {
      theSourceDescription->lockedProperties = (FPXLongArray)(*aProp);
      theSourceDescription->lockedPropertiesIsValid = TRUE;
    } else
      theSourceDescription->lockedPropertiesIsValid = FALSE;

    if (filePtr->GetSourceDescProperty (PID_Title, &aProp)) {
      theSourceDescription->dataObjectTitle = (FPXWideStr)(*aProp);
      theSourceDescription->dataObjectTitleIsValid = TRUE;
    } else
      theSourceDescription->dataObjectTitleIsValid = FALSE;

    if (filePtr->GetSourceDescProperty (PID_LastModifier, &aProp)) {
      theSourceDescription->lastModifier = (FPXWideStr)(*aProp);
      theSourceDescription->lastModifierIsValid = TRUE;
    } else
      theSourceDescription->lastModifierIsValid = FALSE;

    if (filePtr->GetSourceDescProperty (PID_RevisionNumber, &aProp)) {
      theSourceDescription->revisionNumber = (long)(*aProp);
      theSourceDescription->revisionNumberIsValid = TRUE;
    } else
      theSourceDescription->revisionNumberIsValid = FALSE;

    if (filePtr->GetSourceDescProperty (PID_CreationTime, &aProp)) {
      theSourceDescription->creationDate = *(FPXfiletime *)&(*aProp);
      theSourceDescription->creationDateIsValid = TRUE;
    } else
      theSourceDescription->creationDateIsValid = FALSE;

    if (filePtr->GetSourceDescProperty (PID_ModificationTime, &aProp)) {
      theSourceDescription->modificationDate = *(FPXfiletime *)&(*aProp);
      theSourceDescription->modificationDateIsValid = TRUE;
    } else
      theSourceDescription->modificationDateIsValid = FALSE;

    if (filePtr->GetSourceDescProperty (PID_CreatingApplication, &aProp)) {
      theSourceDescription->creatingApplication = (FPXWideStr)(*aProp);
      theSourceDescription->creatingApplicationIsValid = TRUE;
    } else
      theSourceDescription->creatingApplicationIsValid = FALSE;

    if (filePtr->GetSourceDescProperty (PID_Status, &aProp)) {
      long longStatus = (long)(*aProp);
      // PLEASE WINDOWS TESTER: the following line may be little endian/big endian dependent...
      theSourceDescription->status.existenceData = (FPXImageExistence)((longStatus) >> 16);
      theSourceDescription->status.permission    = (FPXImagePermission)((longStatus) & 0xFFFF);
    }
    
    if (filePtr->GetSourceDescProperty (PID_Creator, &aProp))
      theSourceDescription->creator = (long)(*aProp);

    if (filePtr->GetSourceDescProperty (PID_Users, &aProp))
      theSourceDescription->users = (FPXLongArray)(*aProp);

    if (filePtr->GetSourceDescProperty (PID_CachedImageHeight, &aProp)) {
      theSourceDescription->height = (long)(*aProp);
      theSourceDescription->heightIsValid = TRUE;
    } else
      theSourceDescription->heightIsValid = FALSE;

    if (filePtr->GetSourceDescProperty (PID_CachedImageWidth, &aProp)) {
      theSourceDescription->width = (long)(*aProp);
      theSourceDescription->widthIsValid = TRUE;
    } else
      theSourceDescription->widthIsValid = FALSE;
  } else
    err = FPX_NOT_A_VIEW;
  
  return err;
}


//  ----------------------------------------------------------------------------
FPXStatus PFlashPixImageView::SetResultPropertySet(FPXImageDescription* theResultDescription)
{
  FPXStatus err = FPX_OK;

  if (filePtr) {
    OLEProperty* aProp;
    
    if (filePtr->SetResultDescProperty (PID_ImageID, TYP_ImageID, &aProp))
      *aProp = (CLSID *)&(theResultDescription->dataObjectID);

    if (theResultDescription->lockedPropertiesIsValid)
      if (filePtr->SetResultDescProperty (PID_LockedPropertyList, TYP_LockedPropertyList, &aProp))
        *aProp = (FPXLongArray)(theResultDescription->lockedProperties);

    if (theResultDescription->dataObjectTitleIsValid)
      if (filePtr->SetResultDescProperty (PID_Title, TYP_Title, &aProp))
        *aProp = (FPXWideStr)(theResultDescription->dataObjectTitle);

    if (theResultDescription->lastModifierIsValid)
      if (filePtr->SetResultDescProperty (PID_LastModifier, TYP_LastModifier, &aProp))
        *aProp = (FPXWideStr)(theResultDescription->lastModifier);

    if (theResultDescription->revisionNumberIsValid)
      if (filePtr->SetResultDescProperty (PID_RevisionNumber, TYP_RevisionNumber, &aProp))
        *aProp = (long)(theResultDescription->revisionNumber);

    if (theResultDescription->creationDateIsValid)
      if (filePtr->SetResultDescProperty (PID_CreationTime, TYP_CreationTime, &aProp))
        *aProp = *(FILETIME *)&(theResultDescription->creationDate);

    if (theResultDescription->modificationDateIsValid)
      if (filePtr->SetResultDescProperty (PID_ModificationTime, TYP_ModificationTime, &aProp))
        *aProp = *(FILETIME *)&(theResultDescription->modificationDate);

    if (theResultDescription->creatingApplicationIsValid)
      if (filePtr->SetResultDescProperty (PID_CreatingApplication, TYP_CreatingApplication, &aProp))
        *aProp = (FPXWideStr)(theResultDescription->creatingApplication);

    // PLEASE WINDOWS TESTER: the following line may be little endian/big endian dependent...
    long longStatus = (theResultDescription->status.existenceData << 16) | theResultDescription->status.permission;
    if (filePtr->SetResultDescProperty (PID_Status, TYP_Status, &aProp))
      *aProp = (long)(longStatus);

    if (filePtr->SetResultDescProperty (PID_Creator, TYP_Creator, &aProp))
      *aProp = (long)(theResultDescription->creator);

    if (filePtr->SetResultDescProperty (PID_Users, TYP_Users, &aProp))
      *aProp = (FPXLongArray)(theResultDescription->users);

    if (theResultDescription->heightIsValid)
      if (filePtr->SetResultDescProperty (PID_CachedImageHeight, TYP_CachedImageHeight, &aProp))
        *aProp = (long)(theResultDescription->height);

    if (theResultDescription->widthIsValid)
      if (filePtr->SetResultDescProperty (PID_CachedImageWidth, TYP_CachedImageWidth, &aProp))
        *aProp = (long)(theResultDescription->width);

    filePtr->Commit();
  } else
    err = FPX_NOT_A_VIEW;

  return err;
}


//  ----------------------------------------------------------------------------
FPXStatus PFlashPixImageView::GetResultPropertySet(FPXImageDescription* theResultDescription)
{
  FPXStatus err = FPX_OK;

  if (filePtr) {
    OLEProperty* aProp;
    
    if (filePtr->GetResultDescProperty (PID_ImageID, &aProp))
      theResultDescription->dataObjectID = *(CLSID*)(*aProp);

    if (filePtr->GetResultDescProperty (PID_LockedPropertyList, &aProp)) {
      theResultDescription->lockedProperties = (FPXLongArray)(*aProp);
      theResultDescription->lockedPropertiesIsValid = TRUE;
    } else
      theResultDescription->lockedPropertiesIsValid = FALSE;

    if (filePtr->GetResultDescProperty (PID_Title, &aProp)) {
      theResultDescription->dataObjectTitle = (FPXWideStr)(*aProp);
      theResultDescription->dataObjectTitleIsValid = TRUE;
    } else
      theResultDescription->dataObjectTitleIsValid = FALSE;

    if (filePtr->GetResultDescProperty (PID_LastModifier, &aProp)) {
      theResultDescription->lastModifier = (FPXWideStr)(*aProp);
      theResultDescription->lastModifierIsValid = TRUE;
    } else
      theResultDescription->lastModifierIsValid = FALSE;

    if (filePtr->GetResultDescProperty (PID_RevisionNumber, &aProp)) {
      theResultDescription->revisionNumber = (long)(*aProp);
      theResultDescription->revisionNumberIsValid = TRUE;
    } else
      theResultDescription->revisionNumberIsValid = FALSE;

    if (filePtr->GetResultDescProperty (PID_CreationTime, &aProp)) {
      theResultDescription->creationDate = *(FPXfiletime *)&(*aProp);
      theResultDescription->creationDateIsValid = TRUE;
    } else
      theResultDescription->creationDateIsValid = FALSE;

    if (filePtr->GetResultDescProperty (PID_ModificationTime, &aProp)) {
      theResultDescription->modificationDate = *(FPXfiletime *)&(*aProp);
      theResultDescription->modificationDateIsValid = TRUE;
    } else
      theResultDescription->modificationDateIsValid = FALSE;

    if (filePtr->GetResultDescProperty (PID_CreatingApplication, &aProp)) {
      theResultDescription->creatingApplication = (FPXWideStr)(*aProp);
      theResultDescription->creatingApplicationIsValid = TRUE;
    } else
      theResultDescription->creatingApplicationIsValid = FALSE;

    if (filePtr->GetResultDescProperty (PID_Status, &aProp)) {
      long longStatus = (long)(*aProp);
      // PLEASE WINDOWS TESTER: the following line may be little endian/big endian dependent...
      theResultDescription->status.existenceData = (FPXImageExistence)((longStatus) >> 16);
      theResultDescription->status.permission    = (FPXImagePermission)((longStatus) & 0xFFFF);
    }
    
    if (filePtr->GetResultDescProperty (PID_Creator, &aProp))
      theResultDescription->creator = (long)(*aProp);

    if (filePtr->GetResultDescProperty (PID_Users, &aProp))
      theResultDescription->users = (FPXLongArray)(*aProp);

    if (filePtr->GetResultDescProperty (PID_CachedImageHeight, &aProp)) {
      theResultDescription->height = (long)(*aProp);
      theResultDescription->heightIsValid = TRUE;
    } else
      theResultDescription->heightIsValid = FALSE;

    if (filePtr->GetResultDescProperty (PID_CachedImageWidth, &aProp)) {
      theResultDescription->width = (long)(*aProp);
      theResultDescription->widthIsValid = TRUE;
    } else
      theResultDescription->widthIsValid = FALSE;
  } else
    err = FPX_NOT_A_VIEW;
  
  return err;
}


//  ----------------------------------------------------------------------------
FPXStatus PFlashPixImageView::SetTransformPropertySet(FPXTransform* theTransform)
{
  FPXStatus err = FPX_OK;

  if (filePtr) {
    OLEProperty* aProp;
    
    if (filePtr->SetTransformProperty (PID_TransformNodeID, TYP_TransformNodeID, &aProp))
      *aProp = (CLSID *)&(theTransform->transformNodeID);

    if (filePtr->SetTransformProperty (PID_OperationClassID, TYP_OperationClassID, &aProp))
      *aProp = (CLSID *)&(theTransform->operationClassID);

    if (theTransform->lockedPropertiesIsValid)
      if (filePtr->SetTransformProperty (PID_LockedPropertyList, TYP_LockedPropertyList, &aProp))
        *aProp = (FPXLongArray)(theTransform->lockedProperties);

    if (theTransform->transformTitleIsValid)
      if (filePtr->SetTransformProperty (PID_Title, TYP_Title, &aProp))
        *aProp = (FPXWideStr)(theTransform->transformTitle);

    if (theTransform->lastModifierIsValid)
      if (filePtr->SetTransformProperty (PID_LastModifier, TYP_LastModifier, &aProp))
        *aProp = (FPXWideStr)(theTransform->lastModifier);

    if (theTransform->revisionNumberIsValid)
      if (filePtr->SetTransformProperty (PID_RevisionNumber, TYP_RevisionNumber, &aProp))
        *aProp = (long)(theTransform->revisionNumber);

    if (theTransform->creationDateIsValid)
      if (filePtr->SetTransformProperty (PID_CreationTime, TYP_CreationTime, &aProp))
        *aProp = *(FILETIME *)&(theTransform->creationDate);

    if (theTransform->modificationDateIsValid)
      if (filePtr->SetTransformProperty (PID_ModificationTime, TYP_ModificationTime, &aProp))
        *aProp = *(FILETIME *)&(theTransform->modificationDate);

    if (theTransform->creatingApplicationIsValid)
      if (filePtr->SetTransformProperty (PID_CreatingApplication, TYP_CreatingApplication, &aProp))
        *aProp = (FPXWideStr)(theTransform->creatingApplication);

    if (filePtr->SetTransformProperty (PID_InputObjectList, TYP_InputObjectList, &aProp))
      *aProp = (FPXLongArray)(theTransform->inputDataObjects);

    if (filePtr->SetTransformProperty (PID_OutputObjectList, TYP_OutputObjectList, &aProp))
      *aProp = (FPXLongArray)(theTransform->outputDataObjects);

    if (filePtr->SetTransformProperty (PID_OperationNumber, TYP_OperationNumber, &aProp))
      *aProp = (long)(theTransform->operationNumber);

    filePtr->Commit();
  } else
    err = FPX_NOT_A_VIEW;
    
  return err;
}



//  ----------------------------------------------------------------------------
FPXStatus PFlashPixImageView::GetTransformPropertySet(FPXTransform* theTransform)
{
  FPXStatus err = FPX_OK;


  if (filePtr) {
    OLEProperty* aProp;
    
    if (filePtr->GetTransformProperty (PID_TransformNodeID, &aProp))
      theTransform->transformNodeID = *(CLSID*)(*aProp);

    if (filePtr->GetTransformProperty (PID_OperationClassID, &aProp))
      theTransform->operationClassID = *(CLSID*)(*aProp);

    if (filePtr->GetTransformProperty (PID_LockedPropertyList, &aProp)) {
      theTransform->lockedProperties = (FPXLongArray)(*aProp);
      theTransform->lockedPropertiesIsValid = TRUE;
    } else
      theTransform->lockedPropertiesIsValid = FALSE;

    if (filePtr->GetTransformProperty (PID_Title, &aProp)) {
      theTransform->transformTitle = (FPXWideStr)(*aProp);
      theTransform->transformTitleIsValid = TRUE;
    } else
      theTransform->transformTitleIsValid = FALSE;

    if (filePtr->GetTransformProperty (PID_LastModifier, &aProp)) {
      theTransform->lastModifier = (FPXWideStr)(*aProp);
      theTransform->lastModifierIsValid = TRUE;
    } else
      theTransform->lastModifierIsValid = FALSE;

    if (filePtr->GetTransformProperty (PID_RevisionNumber, &aProp)) {
      theTransform->revisionNumber = (long)(*aProp);
      theTransform->revisionNumberIsValid = TRUE;
    } else
      theTransform->revisionNumberIsValid = FALSE;

    if (filePtr->GetTransformProperty (PID_CreationTime, &aProp)) {
      theTransform->creationDate = *(FPXfiletime *)&(*aProp);
      theTransform->creationDateIsValid = TRUE;
    } else
      theTransform->creationDateIsValid = FALSE;

    if (filePtr->GetTransformProperty (PID_ModificationTime, &aProp)) {
      theTransform->modificationDate = *(FPXfiletime *)&(*aProp);
      theTransform->modificationDateIsValid = TRUE;
    } else
      theTransform->modificationDateIsValid = FALSE;

    if (filePtr->GetTransformProperty (PID_CreatingApplication, &aProp)) {
      theTransform->creatingApplication = (FPXWideStr)(*aProp);
      theTransform->creatingApplicationIsValid = TRUE;
    } else
      theTransform->creatingApplicationIsValid = FALSE;

    if (filePtr->GetTransformProperty (PID_InputObjectList, &aProp))
      theTransform->inputDataObjects = (FPXLongArray)(*aProp);

    if (filePtr->GetTransformProperty (PID_OutputObjectList, &aProp))
      theTransform->outputDataObjects = (FPXLongArray)(*aProp);

    if (filePtr->GetTransformProperty (PID_OperationNumber, &aProp))
      theTransform->operationNumber = (long)(*aProp);
  } else
    err = FPX_NOT_A_VIEW;

  return err;
}


//  ------------------------------------------------------------------------------------
FPXStatus PFlashPixImageView::SetOperationPropertySet(FPXOperation* theOperation)
{
  FPXStatus err = FPX_OK;


  if (filePtr) {
    OLEProperty* aProp;
    
    if (filePtr->SetOperationProperty (PID_OperationID, TYP_OperationID, &aProp))
      *aProp = (CLSID *)&(theOperation->operationID);

    filePtr->Commit();
  } else
    err = FPX_NOT_A_VIEW;
    
  return err;
}


//  ------------------------------------------------------------------------------------
FPXStatus PFlashPixImageView::GetOperationPropertySet(FPXOperation* theOperation)
{
  FPXStatus err = FPX_OK;


  if (filePtr) {
    OLEProperty* aProp;
    
    if (filePtr->GetOperationProperty (PID_OperationID, &aProp))
      theOperation->operationID = *(CLSID*)(*aProp);
  } else
    err = FPX_NOT_A_VIEW;
  
  return err;
}



//  ------------------------------------------------------------------------------------
FPXStatus PFlashPixImageView::SetExtensionListPropertySet(FPXExtensionDescription* /*theExtension*/,
                                                          char* /*extensionName*/)
{
  return FPX_UNIMPLEMENTED_FUNCTION;
}


//  ------------------------------------------------------------------------------------
FPXStatus PFlashPixImageView::GetExtensionListPropertySet(FPXExtensionDescription* /*theExtension*/,
                                                          char* /*extensionName*/)
{
  return FPX_UNIMPLEMENTED_FUNCTION;
}


//  ------------------------------------------------------------------------------------
//  Baseline Viewing Transform Properties: get the currently used properties
//  ------------------------------------------------------------------------------------


// Region Of Interest

FPXStatus PFlashPixImageView::SetImageROI (FPXROI* theROI)
{
  if (theROI) {
  
    // Set the new ROI/crop values
    if( SetImageCrop (theROI->left, theROI->top,
      theROI->left + theROI->width, theROI->top + theROI->height))
        return FPX_BAD_COORDINATES;
        
    // Copy the parameter into the internal structure
    
    regionOfInterest      = *theROI;
    transformsHaveBeenEdited  = TRUE;
    hasRegionOfInterest     = TRUE;

  }
  return FPX_OK;
}

        
//  ------------------------------------------------------------------------------------
FPXStatus PFlashPixImageView::GetImageROI (FPXROI* theROI)
{
  // Copy the internal structure into the parameter
  *theROI = regionOfInterest;

  return FPX_OK;
}


//  ------------------------------------------------------------------------------------
// Filtering
FPXStatus PFlashPixImageView::SetImageFilteringValue (FPXFilteringValue* theFiltering)
{
  if (theFiltering) {
    // Copy the parameter into the internal structure
    filtering = *theFiltering;
    // transformsHaveBeenEdited = TRUE;
    hasFiltering            = TRUE;
    
    // Update the filtering data of the parent object
    SetFiltering (*theFiltering);
  }
  
  return FPX_OK;
}        


//  ------------------------------------------------------------------------------------
FPXStatus PFlashPixImageView::GetImageFilteringValue (FPXFilteringValue* theFiltering)
{
  // Copy the internal structure into the parameter
  *theFiltering = filtering;

  return FPX_OK;
}


//  ------------------------------------------------------------------------------------
// Spatial Orientation
FPXStatus PFlashPixImageView::SetImageAffineMatrix (FPXAffineMatrix* theAffineMatrix)
{
  if (theAffineMatrix) {
    // Copy the parameter into the internal structure
    affineMatrix = *theAffineMatrix;
    // transformsHaveBeenEdited = TRUE;
    hasAffineMatrix         = TRUE;
      
    // Update the positionning data of the parent object
    // The transformation is limited to the Baseline case, i.e., 2D affine transformation  (see Appendix A, par. A.3.2)
    TransfoPerspective normPos (affineMatrix.a11, affineMatrix.a21, // 2D scale, rotation and skewing 
                   affineMatrix.a12, affineMatrix.a22,
                   affineMatrix.a14, affineMatrix.a24,  // Translation  
                   affineMatrix.a31, affineMatrix.a32); // Perspective
    TransfoPerspective translat ((float)1.0, (float)0.0,(float) 0.0,(float) 1.0, -regionOfInterest.left, -regionOfInterest.top);
    position = normPos * translat;
  }
  
  return FPX_OK;
}


//  ------------------------------------------------------------------------------------
FPXStatus PFlashPixImageView::GetImageAffineMatrix (FPXAffineMatrix*  theAffineMatrix)
{
  // Copy the internal structure into the parameter
  *theAffineMatrix = affineMatrix;

  return FPX_OK;
}


//  ------------------------------------------------------------------------------------
// Result Aspect Ratio
FPXStatus PFlashPixImageView::SetImageResultAspectRatio (FPXResultAspectRatio* theAspectRatio)
{
  if (theAspectRatio) {
    // Copy the parameter into the internal structure
    aspectRatio = *theAspectRatio;
    transformsHaveBeenEdited  = TRUE;
    hasAspectRatio          = TRUE;

    // CAUTION: after such a call, the aspect ratio is never recomputed automatically bu the Toolkit
    // We use only the dimension set by the user from now.
    resizeFinalDimensions = FALSE;

    // Update the resulting dimensions of the parent object
    finalWidth = aspectRatio * finalHeight;
  }

  return FPX_OK;
}


//  ------------------------------------------------------------------------------------
FPXStatus PFlashPixImageView::GetImageResultAspectRatio (FPXResultAspectRatio* theAspectRatio)
{
  // Copy the internal structure into the parameter
  *theAspectRatio = aspectRatio;

  return FPX_OK;
}


//  ------------------------------------------------------------------------------------
// ColorTwist Matrix
FPXStatus PFlashPixImageView::SetImageColorTwistMatrix (FPXColorTwistMatrix* theColorTwistMatrix)
{
  if (theColorTwistMatrix) {
    // Copy the parameter into the internal structure
    colorTwistMatrix = *theColorTwistMatrix;
    // transformsHaveBeenEdited = TRUE;
    hasColorTwistMatrix       = TRUE;
    
    // Update the color twist data of the parent object
    PColorTwist twistMatrix (*theColorTwistMatrix);

    SetColorTwist (&twistMatrix);
  }
  return FPX_OK;
}

        
//  ------------------------------------------------------------------------------------
FPXStatus PFlashPixImageView::GetImageColorTwistMatrix (FPXColorTwistMatrix* theColorTwistMatrix)
{
  // Copy the internal structure into the parameter
  *theColorTwistMatrix = colorTwistMatrix;

  return FPX_OK;
}


//  ------------------------------------------------------------------------------------
// Contrast Adjustement
FPXStatus PFlashPixImageView::SetImageContrastAdjustment (FPXContrastAdjustment* theContrastAdjustment)
{
  if (theContrastAdjustment) {
    // Copy the parameter into the internal structure
    contrastAdjustment = *theContrastAdjustment;
    // transformsHaveBeenEdited = TRUE;
    hasContrastAdjustment       = TRUE;
    
    // Update the contrast data of the parent object
    SetContrast (*theContrastAdjustment);
  }

  return FPX_OK;
}

        
//  ------------------------------------------------------------------------------------
FPXStatus PFlashPixImageView::GetImageContrastAdjustment (FPXContrastAdjustment* theContrastAdjustment)
{
  // Copy the internal structure into the parameter
  *theContrastAdjustment = contrastAdjustment;

  return FPX_OK;
}


//  ------------------------------------------------------------------------------------
//  Baseline Viewing Transform Properties: load and save from the file
//  ------------------------------------------------------------------------------------

void PFlashPixImageView::InitTransformProperties ()
{
  // Geometric parameters
  float i0, j0, i1, j1;
  GetOutlineRectangle (&i0, &j0, &i1, &j1);

  aspectRatio = (i1 - i0)/(j1 - j0);

  regionOfInterest.left   = (float)0.0;
  regionOfInterest.top    = (float)0.0;
  regionOfInterest.width  = aspectRatio;
  regionOfInterest.height = (float)1.0;

  affineMatrix.a11 =(float) 1.0; affineMatrix.a12 = (float)0.0; affineMatrix.a13 =(float) 0.0; affineMatrix.a14 =(float) 0.0;
  affineMatrix.a21 = (float)0.0; affineMatrix.a22 = (float)1.0; affineMatrix.a23 = (float)0.0; affineMatrix.a24 = (float)0.0;
  affineMatrix.a31 = (float)0.0; affineMatrix.a32 = (float)0.0; affineMatrix.a33 = (float)1.0; affineMatrix.a34 = (float)0.0;
  affineMatrix.a41 = (float)0.0; affineMatrix.a42 = (float)0.0; affineMatrix.a43 =(float) 0.0; affineMatrix.a44 = (float)1.0;

  // Display parameters
  filtering = (float)0.0;
  contrastAdjustment = (float)1.0;

  colorTwistMatrix.byy  = (float)1.0; colorTwistMatrix.byc1  = (float)0.0; colorTwistMatrix.byc2  = (float)0.0; colorTwistMatrix.dummy1_zero = (float)0.0;
  colorTwistMatrix.bc1y = (float)0.0; colorTwistMatrix.bc1c1 = (float)1.0; colorTwistMatrix.bc1c2 = (float)0.0; colorTwistMatrix.dummy2_zero = (float)0.0;
  colorTwistMatrix.bc2y = (float)0.0; colorTwistMatrix.bc2c1 = (float)0.0; colorTwistMatrix.bc2c2 = (float)1.0; colorTwistMatrix.dummy3_zero = (float)0.0;
  colorTwistMatrix.dummy4_zero = (float)0.0; colorTwistMatrix.dummy5_zero =(float) 0.0; colorTwistMatrix.dummy6_zero = (float)0.0; colorTwistMatrix.dummy7_one = (float)1.0;
  
  // By default, none of these parameters have to be taken into account when saving
  hasRegionOfInterest   = FALSE;
  hasAffineMatrix     = FALSE;
  hasAspectRatio      = FALSE;
  hasFiltering      = FALSE;
  hasColorTwistMatrix   = FALSE;
  hasContrastAdjustment = FALSE;
}


//  ------------------------------------------------------------------------------------
// Region Of Interest
FPXStatus PFlashPixImageView::SaveImageROI ()
{
  FPXStatus error = FPX_OK;
  

  if (!filePtr)
    // We don't create a FPX Image View at that level if it doesn't exist
    error = FPX_NOT_A_VIEW;
  else if (hasRegionOfInterest  && transformsHaveBeenEdited) {
    OLEProperty* aProp;
  
    if(readOnlyFile)
      return FPX_FILE_WRITE_ERROR;
    // Create an intermediate FPXRealArray pointing to the FPXROI structure

    FPXRealArray mat;

    mat.length = sizeof(FPXROI)/sizeof(float);
    mat.ptr    = (float*)(&regionOfInterest);
    assert (mat.length == 4);

    // Set the property
    if (filePtr->SetTransformProperty (PID_RectOfInterest, TYP_RectOfInterest, &aProp))
    {
      *aProp = (FPXRealArray)(mat);
      // transformsHaveBeenEdited = TRUE;
    }
    else
      error = FPX_FILE_WRITE_ERROR;

    // Write it on disk
    filePtr->Commit();
  }
  
  return error;
}


//  ------------------------------------------------------------------------------------
FPXStatus PFlashPixImageView::LoadImageROI ()
{
  FPXStatus error = FPX_OK;
  
  if (!filePtr)
    // We don't create a FPX Image View at that level if it doesn't exist
    error = FPX_NOT_A_VIEW;
  else {
    OLEProperty* aProp;
    // Create an intermediate FPXRealArray to store the property
    FPXRealArray mat;

    if (filePtr->GetTransformProperty (PID_RectOfInterest, &aProp)) {

      mat = (FPXRealArray)(*aProp);
      assert (mat.length == 4);

      // Transfert the FPXRealArray values into the FPXROI structure
      BlockMove(mat.ptr,&regionOfInterest,sizeof(FPXROI));
      hasRegionOfInterest = TRUE;

      // Resize bogus ROI... (too common to be avoided... unfortunately...)
      if ((regionOfInterest.left  > aspectRatio) || (regionOfInterest.top    > 1.0) || 
        (regionOfInterest.width > aspectRatio) || (regionOfInterest.height > 1.0)) {
        
        if ((regionOfInterest.width > originWidth) || (regionOfInterest.height > originHeight)) {
          // The crop is certainly given in high res pixels...
          regionOfInterest.left   /= (float)(image->height);
          regionOfInterest.top    /= (float)(image->height);
          regionOfInterest.width  /= (float)(image->height);
          regionOfInterest.height /= (float)(image->height);
        } else {
          // The crop is certainly given in the default size
          regionOfInterest.left   /= originHeight;
          regionOfInterest.top    /= originHeight;
          regionOfInterest.width  /= originHeight;
          regionOfInterest.height /= originHeight;
        }
      }
    } else
      error = FPX_FILE_READ_ERROR;
  }
  
  return error;
}


//  ------------------------------------------------------------------------------------
// Filtering
FPXStatus PFlashPixImageView::SaveImageFilteringValue ()
{
  FPXStatus error = FPX_OK;
  Boolean status = TRUE;
  
  if (!filePtr)
    // We don't create a FPX Image View at that level if it doesn't exist
    error = FPX_NOT_A_VIEW;
  else if (hasFiltering) {
    // check to see if read only
    if(readOnlyFile)
      return FPX_FILE_WRITE_ERROR;
      
    OLEProperty* aProp;
    // Set the property
    if (filePtr->SetTransformProperty (PID_Filtering, TYP_Filtering, &aProp))
    {
      transformsHaveBeenEdited  = TRUE;
      *aProp = (float)(filtering);
    }
    else
    {
      error = FPX_FILE_WRITE_ERROR;
      return error;
    }
      
    // Write it on disk
    status = filePtr->Commit();
    if(status == FALSE)
      error = filePtr->getStatus();
  }
  
  return error;
}


//  ------------------------------------------------------------------------------------
FPXStatus PFlashPixImageView::LoadImageFilteringValue ()
{
  FPXStatus error = FPX_OK;

  if (!filePtr)
    // We don't create a FPX Image View at that level if it doesn't exist
    error = FPX_NOT_A_VIEW;
  else {
    OLEProperty* aProp;
    if (filePtr->GetTransformProperty (PID_Filtering, &aProp)) {
      filtering = (float)(*aProp);
      hasFiltering = TRUE;
    } else
      error = FPX_FILE_READ_ERROR;
  } 
  return error;
}


//  ------------------------------------------------------------------------------------
// Spatial Orientation
FPXStatus PFlashPixImageView::SaveImageAffineMatrix ()

{

  FPXStatus error = FPX_OK;

  if (!filePtr)
    // We don't create a FPX Image View at that level if it doesn't exist
    error = FPX_NOT_A_VIEW;

  else if (hasAffineMatrix) {
    OLEProperty* aProp;
    
    if(readOnlyFile)
      return FPX_FILE_WRITE_ERROR;

    // Create an intermediate FPXRealArray pointing to the FPXAffineMatrix
    FPXRealArray mat;
    mat.length = sizeof(FPXAffineMatrix)/sizeof(float);
    mat.ptr    = (float*)(&affineMatrix);
    assert (mat.length == 16);

    // Set the property

    if (filePtr->SetTransformProperty (PID_SpatialOrientation, TYP_SpatialOrientation, &aProp))
    {
      *aProp = (FPXRealArray)(mat);
      transformsHaveBeenEdited  = TRUE;
    }
    else
      error = FPX_FILE_WRITE_ERROR;

    // Write it on disk
    filePtr->Commit();
  }

  return error;
}


//  ------------------------------------------------------------------------------------
FPXStatus PFlashPixImageView::LoadImageAffineMatrix ()
{
  FPXStatus error = FPX_OK;
  
  if (!filePtr)
    // We don't create a FPX Image View at that level if it doesn't exist
    error = FPX_NOT_A_VIEW;
  else {
    OLEProperty* aProp;
    // Create an intermediate FPXRealArray to store the property
    FPXRealArray mat;
    if (filePtr->GetTransformProperty (PID_SpatialOrientation, &aProp)) {
      mat = (FPXRealArray)(*aProp);
      assert (mat.length == 16);
      // Transfert the FPXRealArray values into the FPXAffineMatrix structure
      BlockMove(mat.ptr,&affineMatrix,sizeof(FPXAffineMatrix));
      hasAffineMatrix = TRUE;
    } else
      error = FPX_FILE_READ_ERROR;
  }

  return error;
}


//  ------------------------------------------------------------------------------------
// Result Aspect Ratio
FPXStatus PFlashPixImageView::SaveImageResultAspectRatio ()
{
  FPXStatus error = FPX_OK;
  if (!filePtr)
    // We don't create a FPX Image View at that level if it doesn't exist
    error = FPX_NOT_A_VIEW;
  else if (hasAspectRatio  && transformsHaveBeenEdited && !resizeFinalDimensions) {
    // CAUTION: this property must be saved only if the user sets it, otherwise, it should
    // be left free to move along with the position matrix.
    OLEProperty* aProp; 
      
    if(readOnlyFile)
      return FPX_FILE_WRITE_ERROR;
    
    // Set the property
    if (filePtr->SetTransformProperty (PID_ResultAspectRatio, TYP_ResultAspectRatio, &aProp))
    {
      *aProp = (float)(aspectRatio);
      // transformsHaveBeenEdited = TRUE;
    }
    else
    {
      error = FPX_FILE_WRITE_ERROR;
      return error;
    }
    // Write it on disk
    filePtr->Commit();

  }

  return error;
}


//  ------------------------------------------------------------------------------------
FPXStatus PFlashPixImageView::LoadImageResultAspectRatio ()
{
  FPXStatus error = FPX_OK;
  
  if (!filePtr)
    // We don't create a FPX Image View at that level if it doesn't exist
    error = FPX_NOT_A_VIEW;
  else {
    OLEProperty* aProp;
    if (filePtr->GetTransformProperty (PID_ResultAspectRatio, &aProp)) {
      aspectRatio = (float)(*aProp);
      // CAUTION: if this property has been specified by the user, the result image will not
      // be automatically resized by the Toolkit. We'll use the final crop specified by the user.
      resizeFinalDimensions = FALSE;
      hasAspectRatio      = TRUE;
    } else
      error = FPX_FILE_READ_ERROR;
  }
  
  return error;
}


//  ------------------------------------------------------------------------------------
// ColorTwist Matrix
FPXStatus PFlashPixImageView::SaveImageColorTwistMatrix ()
{
  FPXStatus error = FPX_OK;
  
  if (!filePtr)
    // We don't create a FPX Image View at that level if it doesn't exist
    error = FPX_NOT_A_VIEW;
  else if (hasColorTwistMatrix) {
    OLEProperty* aProp;
  
    if(readOnlyFile)
      return FPX_FILE_WRITE_ERROR;

    // Create an intermediate FPXRealArray pointing to the FPXAffineMatrix
    FPXRealArray mat;
    mat.length = sizeof(FPXAffineMatrix)/sizeof(float);
    mat.ptr    = (float*)(&colorTwistMatrix);
    assert (mat.length == 16);
    // Set the property

    if (filePtr->SetTransformProperty (PID_ColortwistMatrix, TYP_ColortwistMatrix, &aProp))
    {
      *aProp = (FPXRealArray)(mat);
      transformsHaveBeenEdited  = TRUE;
    }
    else
      error = FPX_FILE_WRITE_ERROR;

    // Write it on disk
    filePtr->Commit();
  }

  return error;
}


//  ------------------------------------------------------------------------------------
FPXStatus PFlashPixImageView::LoadImageColorTwistMatrix ()
{
  FPXStatus error = FPX_OK;

  //Initialize to identity
  colorTwistMatrix.byy  = (float)1.0; colorTwistMatrix.byc1  = (float)0.0; colorTwistMatrix.byc2  = (float)0.0; colorTwistMatrix.dummy1_zero = (float)0.0;

  colorTwistMatrix.bc1y = (float)0.0; colorTwistMatrix.bc1c1 = (float)1.0; colorTwistMatrix.bc1c2 = (float)0.0; colorTwistMatrix.dummy2_zero = (float)0.0;

  colorTwistMatrix.bc2y = (float)0.0; colorTwistMatrix.bc2c1 = (float)0.0; colorTwistMatrix.bc2c2 = (float)1.0; colorTwistMatrix.dummy3_zero = (float)0.0;

  colorTwistMatrix.dummy4_zero = (float)0.0; colorTwistMatrix.dummy5_zero =(float) 0.0; colorTwistMatrix.dummy6_zero = (float)0.0; colorTwistMatrix.dummy7_one = (float)1.0;  

  if (!filePtr)
    // We don't create a FPX Image View at that level if it doesn't exist
    error = FPX_NOT_A_VIEW;
  else {
    OLEProperty* aProp;
    // Create an intermediate FPXRealArray to store the property
    FPXRealArray mat;
    if (filePtr->GetTransformProperty (PID_ColortwistMatrix, &aProp)) {
      mat = (FPXRealArray)(*aProp);
      assert (mat.length == 16);
      // Transfert the FPXRealArray values into the FPXAffineMatrix structure
      BlockMove(mat.ptr,&colorTwistMatrix,sizeof(FPXColorTwistMatrix));
      hasColorTwistMatrix = TRUE;
    } else
      error = FPX_FILE_READ_ERROR;
  }
  return error;
}


//  ------------------------------------------------------------------------------------
// Contrast Adjustement
FPXStatus PFlashPixImageView::SaveImageContrastAdjustment ()
{
  FPXStatus error = FPX_OK;
  
  if (!filePtr)
    // We don't create a FPX Image View at that level if it doesn't exist
    error = FPX_NOT_A_VIEW;
  else if (hasContrastAdjustment) {
    OLEProperty* aProp;
    if(readOnlyFile)
      return FPX_FILE_WRITE_ERROR;
      
    // Set the property
    if (filePtr->SetTransformProperty (PID_ContrastAdjust, TYP_ContrastAdjust, &aProp))
    {
      *aProp = (float)(contrastAdjustment);
      transformsHaveBeenEdited  = TRUE;
    }
    else
      error = FPX_FILE_WRITE_ERROR;

    // Write it on disk
    filePtr->Commit();
  }

  return error;
}


//  ------------------------------------------------------------------------------------
FPXStatus PFlashPixImageView::LoadImageContrastAdjustment ()
{
  FPXStatus error = FPX_OK;
  
  if (!filePtr)
    // We don't create a FPX Image View at that level if it doesn't exist
    error = FPX_NOT_A_VIEW;
  else {
    OLEProperty* aProp;
    if (filePtr->GetTransformProperty (PID_ContrastAdjust, &aProp)) {
      contrastAdjustment = (float)(*aProp);
      hasContrastAdjustment = TRUE;
    } else
      error = FPX_FILE_READ_ERROR;
  }
  
  return error;
}
