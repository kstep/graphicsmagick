//  ----------------------------------------------------------------------------
//  MODULE    : FPXBaselineView
//  LANGUAGE  : C++ 
//  CREATOR   : Philippe BOSSUT 
//  CREAT. DATE : Friday, April 5, 1996 
//  DESCRIPTION :  
//  COMMENTS  : 
//  SCCSID      : @(#)fpxlib.cpp  1.3 08:41:22 29 Jan 1997
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------

//  ----------------------------------------------------------------------------

//  Includes
//  --------

#ifndef PageIVUE_h
  #include  "ri_page.h"
#endif

#ifndef OLECommun_h
  #include "olecomm.h"
#endif
#ifndef OLECore_h
  #include "olecore.h"
#endif
#ifndef OLEProperties_h
  #include  "oleprop.h"
#endif

#ifndef FileFlashPixIO_h
  #include "f_fpxio.h"
#endif
#ifndef FileFlashPixView_h
  #include "f_fpxvw.h"
#endif
#ifndef FPXImageView_h
  #include "fpximgvw.h"
#endif
#ifndef FlashPixFormat_h
  #include "fpxformt.h"
#endif
#ifndef FlashPixUtils_h
  #include "fpxutils.h"
#endif

#ifndef FPXBaselineView_h
  #include "fpxlib.h"
#endif

#ifndef BufferDesc_h
  #include "buffdesc.h"
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

//  ----------------------------------------------------------------------------
//  External functions used by C code : 
//  This is the interface of the Baseline FlashPix Viewing Toolkit
//  ----------------------------------------------------------------------------


// World tools :
     
FPXStatus FPX_CreateWorld (
              FPXWorld**    theWorld, 
              float         width, 
              float         height,
              FPXColorspace backgroundColorspace,
              FPXBackground backgroundColor)
{
  FPXStatus status = FPX_OK;

  *theWorld = new ViewWorld (width, height, 0);

  if (!*theWorld)
    status = FPX_SEVER_INIT_ERROR;
  else
    FPX_SetViewBackgroundColor(backgroundColorspace,backgroundColor);

  return status;
}

FPXStatus FPX_DeleteWorld (
              FPXWorld*     theWorld)
{
  if (theWorld == NULL) {
    return FPX_INVALID_FPX_HANDLE;
  } else {
    delete theWorld;
    return FPX_OK;
  }
}

// Management of the list of images :

FPXStatus FPX_AddImage (
              FPXWorld*         theWorld, 
              FPXImageInWorld** theImage,
              FPXImageHandle*   theFPX)
{

  FPXStatus status = FPX_OK;
  
  *theImage = NULL;
  if (theWorld) {
    if (theWorld->AddImage (theFPX))
      status = FPX_INVALID_FPX_HANDLE;
    else
      theWorld->Current (theImage);
  }
  else
    status = FPX_INVALID_FPX_HANDLE;
    
  return status;
}
     
FPXStatus FPX_DeleteImage (
              FPXWorld*        theWorld, 
              FPXImageInWorld* theImage)
{
  FPXStatus status = FPX_OK;
  
  if (theWorld) {
    if (theWorld->DeleteImage (theImage))
      status = FPX_INVALID_FPX_HANDLE;
  }
  else
    status = FPX_INVALID_FPX_HANDLE;
    
  return status;
}

FPXStatus FPX_GetFirstImage (
              FPXWorld*         theWorld, 
              FPXImageInWorld** theImage)
{
  FPXStatus status = FPX_OK;
  
  if (theWorld) {
    if (theWorld->First (theImage))
      status = FPX_INVALID_FPX_HANDLE;
  }
  else
    status = FPX_INVALID_FPX_HANDLE;
    
  return status;
}
     
FPXStatus FPX_GetLastImage (
              FPXWorld*         theWorld, 
              FPXImageInWorld** theImage)
{
  FPXStatus status = FPX_OK;
  
  if (theWorld) {
    if (theWorld->Last (theImage))
      status = FPX_INVALID_FPX_HANDLE;
  }
  else
    status = FPX_INVALID_FPX_HANDLE;
    
  return status;
}
     
FPXStatus FPX_GetCurrentImage (
              FPXWorld*         theWorld, 
              FPXImageInWorld** theImage)
{
  FPXStatus status = FPX_OK;
  
  if (theWorld) {
    if (theWorld->Current (theImage))
      status = FPX_INVALID_FPX_HANDLE;
  }
  else
    status = FPX_INVALID_FPX_HANDLE;
    
  return status;
}

FPXStatus FPX_NextImage (
              FPXWorld*         theWorld, 
              FPXImageInWorld** theImage)
{
  FPXStatus status = FPX_OK;
  
  if (theWorld) {
    if (theWorld->Next (theImage))
      status = FPX_INVALID_FPX_HANDLE;
  }
  else
    status = FPX_INVALID_FPX_HANDLE;
    
  return status;
}
     
FPXStatus FPX_PreviousImage (
              FPXWorld*         theWorld, 
              FPXImageInWorld** theImage)
{
  FPXStatus status = FPX_OK;
  
  if (theWorld) {
    if (theWorld->Previous (theImage))
      status = FPX_INVALID_FPX_HANDLE;
  }
  else
    status = FPX_INVALID_FPX_HANDLE;
    
  return status;
}

// Image positioning tools :

FPXStatus FPX_UseAlphaChannel (
              FPXImageInWorld* theImage,
              FPXbool          useAlphaChannel)
{
  FPXStatus status = FPX_OK;
  
  if (theImage) {
    if (theImage->UseAlphaChannel (useAlphaChannel))
      status = FPX_INVALID_FPX_HANDLE;
  }
  else
    status = FPX_INVALID_FPX_HANDLE;
    
  return status;
}

FPXStatus FPX_InvertAlphaChannel (
              FPXImageInWorld* theImage,
              FPXbool          inverseAlpha)
{
  FPXStatus status = FPX_OK;
  
  if (theImage) {
    if (theImage->InvertAlphaChannel (inverseAlpha))
      status = FPX_INVALID_FPX_HANDLE;
  }
  else
    status = FPX_INVALID_FPX_HANDLE;
    
  return status;
}

FPXStatus FPX_SetImageSize (
              FPXImageInWorld* theImage, 
              float            width, 
              float            height)
{
  FPXStatus status = FPX_OK;
  
  if (theImage) {
    if (theImage->SetImageSize (width, height))
      status = FPX_INVALID_FPX_HANDLE;
  }
  else
    status = FPX_INVALID_FPX_HANDLE;
    
  return status;
}
     
FPXStatus FPX_SetImageTransform (
              FPXImageInWorld* theImage, 
              float x0,  float y0, 
              float m11, float m12, float m21, float m22)
{
  FPXStatus status = FPX_OK;
  
  if (theImage) 
    theImage->SetPosition (x0, y0, m11, m12, m21, m22);
  else
    status = FPX_INVALID_FPX_HANDLE;
    
  return status;
}


FPXStatus FPX_ApplyImageTransform (
              FPXImageInWorld* theImage, 
              float x0,  float y0, 
              float m11, float m12, float m21, float m22)
{
  FPXStatus status = FPX_OK;
  
  if (theImage)
    theImage->ApplyTransform (x0, y0, m11, m12, m21, m22);
  else
    status = FPX_INVALID_FPX_HANDLE;
    
  return status;
}

  
FPXStatus FPX_TranslateImage (
              FPXImageInWorld* theImage, 
              float dx, float dy)
{
  FPXStatus status = FPX_OK;
  
  if (theImage) 
    theImage->Translate (dx, dy);
  else
    status = FPX_INVALID_FPX_HANDLE;
    
  return status;
}

     
FPXStatus FPX_RotateImage (
              FPXImageInWorld* theImage, 
              float x0, float y0,
              float theta)
{
  FPXStatus status = FPX_OK;
  
  if (theImage) 
    theImage->Rotate (x0, y0, theta);
  else
    status = FPX_INVALID_FPX_HANDLE;
    
  return status;
}
     
     
FPXStatus FPX_ScaleImage (
              FPXImageInWorld* theImage, 
              float x0, float y0,
              float sx, float sy)
{
  FPXStatus status = FPX_OK;
  
  if (theImage) 
    theImage->Scale (x0, y0, sx, sy);
  else
    status = FPX_INVALID_FPX_HANDLE;
    
  return status;
}

     
FPXStatus FPX_ShearHorizontal (
              FPXImageInWorld* theImage, 
              float x0, float y0, 
              float sh)
{
  FPXStatus status = FPX_OK;
  
  if (theImage) 
    theImage->ShearHorizontal (x0, y0, sh);
  else
    status = FPX_INVALID_FPX_HANDLE;
    
  return status;
}


FPXStatus FPX_ShearVertical (
              FPXImageInWorld* theImage, 
              float x0, float y0, 
              float sh)
{
  FPXStatus status = FPX_OK;
  
  if (theImage)
    theImage->ShearVertical (x0, y0, sh);
  else
    status = FPX_INVALID_FPX_HANDLE;
    
  return status;
}


FPXStatus FPX_FlipHorizontal (
              FPXImageInWorld* theImage, 
              float x0, float y0)
{
  FPXStatus status = FPX_OK;
  
  if (theImage) 
    theImage->FlipHorizontal (x0, y0);
  else
    status = FPX_INVALID_FPX_HANDLE;
    
  return status;
}


FPXStatus FPX_FlipVertical (
              FPXImageInWorld* theImage, 
              float x0, float y0)
{
  FPXStatus status = FPX_OK;

  if (theImage)
    theImage->FlipVertical (x0, y0);
  else
    status = FPX_INVALID_FPX_HANDLE;
    
  return status;
}


FPXStatus FPX_GetOutlineParallelogram (
              FPXImageInWorld* theImage, 
              float* x0, float* y0, 
              float* x1, float* y1,
              float* x2, float* y2, 
              float* x3, float* y3)
{
  FPXStatus status = FPX_OK;
  
  if (theImage)
    theImage->GetOutlineParallelogram (x0, y0, x1, y1, x2, y2, x3, y3);
  else
    status = FPX_INVALID_FPX_HANDLE;
    
  return status;
}
     
FPXStatus FPX_GetOutlineRectangle (
              FPXImageInWorld* theImage, 
              float* x0, float* y0, 
              float* x1, float* y1)
{
  FPXStatus status = FPX_OK;
  
  if (theImage) 
    theImage->GetOutlineRectangle (x0, y0, x1, y1);
  else
    status = FPX_INVALID_FPX_HANDLE;
    
  return status;
}

     
FPXStatus FPX_GetOrigin (
              FPXImageInWorld* theImage, 
              float* x0, float* y0)
{
  FPXStatus status = FPX_OK;
  
  if (theImage) 
    theImage->GetOrigin (x0, y0);
  else
    status = FPX_INVALID_FPX_HANDLE;
    
  return status;
}


FPXStatus FPX_SetImageCrop (
              FPXImageInWorld* theImage, 
              float x0, float y0, 
              float x1, float y1)
{
  FPXStatus status = FPX_OK;
  
  if (theImage) 
    status = theImage->SetImageCrop (x0, y0, x1, y1);
  else
    status = FPX_INVALID_FPX_HANDLE;
    
  return status;
}


FPXStatus FPX_ResetImageCrop (
              FPXImageInWorld* theImage)
{
  FPXStatus status = FPX_OK;
  
  if (theImage)
    theImage->ResetImageCrop ();
  else
    status = FPX_INVALID_FPX_HANDLE;
    
  return status;
}
     
     

// View Window  tools :

FPXStatus FPX_CreateWindow (
              FPXWorld*    theWorld, 
              FPXWindow**  theWindow, 
              float x0,    float y0, 
              float width, float height, 
              float resolution)
{
  FPXStatus status = FPX_OK;
  
  if (!theWorld)
    status = FPX_INVALID_FPX_HANDLE;
  else {
    // convert resolution in dot per unit into resol in dot per mm
    // CAUTION : resolution unit is the inverse of a length, 
    // so ConvertToUnit convert resolution in dot per mmÉ
    resolution = Toolkit_ConvertToUnit(resolution);

    *theWindow = new ViewWindow (theWorld, x0, y0, width, height, resolution);
    if (!*theWindow)
      status = FPX_SEVER_INIT_ERROR;
  }
  
  return status;
}

FPXStatus FPX_DeleteWindow (
              FPXWindow* theWindow)
{
  if (theWindow == NULL) {
    return FPX_INVALID_FPX_HANDLE;
  } else {
    delete theWindow;
    return FPX_OK;
  }
}

FPXStatus FPX_ReadWindowSample (
              FPXWindow*    theWindow, 
              long x0,      long y0,
              FPXImageDesc* windowBufferInfo)
{
  FPXStatus status = FPX_OK;
  if (!theWindow)
    status = FPX_INVALID_FPX_HANDLE;
  else {
    FPXBufferDesc sample(windowBufferInfo,4,4);
    if (sample.Get32BitsBuffer() == NULL) {
      return FPX_OBJECT_CREATION_FAILED;
    }
    if (sample.GetBaselineColorSpace() == NON_AUTHORIZED_SPACE)
      status = FPX_INVALID_IMAGE_DESC;
    else {
      switch (theWindow->ReadSample (x0, y0, sample.Get32BitsBuffer(), sample.GetBaselineColorSpace())) {
        case 0:
          sample.UpdateDescriptor();
          break;
        default :
          status = FPX_FILE_READ_ERROR;
          break;
      }
    }
  }
  return status;
}

FPXStatus FPX_RefreshWindow (
              FPXWindow*    theWindow, 
              FPXImageDesc* windowBufferInfo)
{
  FPXStatus status = FPX_OK;
  if (!theWindow)
    status = FPX_INVALID_FPX_HANDLE;
  else {
    long width, height;
    //theWindow->GetWindowDefinition (&width, &height);
    float tmpx0, tmpy0, tmpwidth, tmpheight, tmpresolution;
    theWindow->GetWindowDefinition (&tmpx0, &tmpy0, &tmpwidth, &tmpheight, &tmpresolution);

    theWindow->WorldToWindow(tmpx0+tmpwidth,tmpy0+tmpheight,&width,
              &height);
    FPXBufferDesc map(windowBufferInfo,width,height);
    if (map.Get32BitsBuffer() == NULL) {
      return FPX_OBJECT_CREATION_FAILED;
    }
    if (map.GetBaselineColorSpace() == NON_AUTHORIZED_SPACE)
      status = FPX_INVALID_IMAGE_DESC;
    else {
      status = theWindow->Refresh (map.Get32BitsBuffer(),map.GetBaselineColorSpace(),width,height);
      if (status == FPX_OK)
        map.UpdateDescriptor();
    }
  }
  return status;
}


FPXStatus FPX_TranslateWindow (
              FPXWindow* theWindow, 
              float  dx, float  dy)
{
  FPXStatus status = FPX_OK;
  
  if (theWindow)
    theWindow->Translate (dx, dy);
  else
    status = FPX_INVALID_FPX_HANDLE;
    
  return status;
}


FPXStatus FPX_ResizeWindow (
              FPXWindow*   theWindow, 
              float width, float height)
{
  FPXStatus status = FPX_OK;
  
  if (theWindow)
    theWindow->Resize (width, height);
  else
    status = FPX_INVALID_FPX_HANDLE;
    
  return status;
}


FPXStatus FPX_ZoomWindow (
              FPXWindow* theWindow, 
              float      zoomRatio)
{
  FPXStatus status = FPX_OK;
  
  if (theWindow) 
    status = theWindow->Zoom (zoomRatio);
  else
    status = FPX_INVALID_FPX_HANDLE;
    
  return status;
}


FPXStatus FPX_GetWindowDefinition (
              FPXWindow*    theWindow, 
              float* x0,    float* y0, 
              float* width, float* height, 
              float* resolution)
{
  FPXStatus status = FPX_OK;
  
  if (theWindow)
    theWindow->GetWindowDefinition (x0, y0, width, height, resolution);
  else
    status = FPX_INVALID_FPX_HANDLE;
    
  return status;
}


FPXStatus FPX_WindowToWorld (
              FPXWindow* theWindow, 
              long i,    long j, 
              float* x,  float* y)
{
  FPXStatus status = FPX_OK;
  
  if (theWindow) 
    theWindow->WindowToWorld (i, j, x, y);
  else
    status = FPX_INVALID_FPX_HANDLE;
    
  return status;
}


FPXStatus FPX_WorldToWindow (
              FPXWindow* theWindow, 
              float x, float y, 
              long* i, long* j)
{
  FPXStatus status = FPX_OK;
  
  if (theWindow) 
    theWindow->WorldToWindow (x, y, i, j);
  else
    status = FPX_INVALID_FPX_HANDLE;
    
  return status;
}


FPXStatus FPX_SetImageInWorldContrastAdjustment (
              FPXImageInWorld*       theImage,
              FPXContrastAdjustment* theContrastAdjustment)
{
  return ((FPXImageHandle*)(theImage))->SetImageContrastAdjustment(theContrastAdjustment);
}
        
FPXStatus FPX_GetImageInWorldContrastAdjustment (
              FPXImageInWorld*       theImage,
              FPXContrastAdjustment* theContrastAdjustment)
{
  return FPX_GetImageContrastAdjustment ((FPXImageHandle*)(theImage), theContrastAdjustment);
}

FPXStatus FPX_SetImageInWorldColorTwistMatrix (
              FPXImageInWorld*     theImage,
              FPXColorTwistMatrix* theColorTwistMatrix)
{
  return ((FPXImageHandle*)(theImage))->SetImageColorTwistMatrix(theColorTwistMatrix);
}
        
FPXStatus FPX_GetImageInWorldColorTwistMatrix (
              FPXImageInWorld*     theImage,
              FPXColorTwistMatrix* theColorTwistMatrix)
{
  return FPX_GetImageColorTwistMatrix ((FPXImageHandle*)(theImage), theColorTwistMatrix);
}

FPXStatus FPX_SetImageInWorldFilteringValue (
              FPXImageInWorld*   theImage,
              FPXFilteringValue* theFiltering)
{
  FPXStatus status = FPX_OK;
  Boolean filterclamped = FALSE;
  
  if(*theFiltering < -20.0)
  {
    *theFiltering = -20.0;
    filterclamped = TRUE;
  }
  
  if( *theFiltering > 20.0)
  {
    *theFiltering = 20.0;
    filterclamped = TRUE;
  }
  status = ((FPXImageHandle*)(theImage))->SetImageFilteringValue(theFiltering);
  
  if(status == FPX_OK && filterclamped == TRUE)
    status = FPX_W_COORDINATES_OUT_OF_RANGE;
    
  return status;
}
        
FPXStatus FPX_GetImageInWorldFilteringValue (
              FPXImageInWorld*   theImage,
              FPXFilteringValue* theFiltering)
{
  return FPX_GetImageFilteringValue ((FPXImageHandle*)(theImage), theFiltering);
}


FPXStatus FPX_GetImageInWorldAffineMatrix (
      FPXImageInWorld*    theImage,
      FPXAffineMatrix*  mat)
{
  FPXStatus status = FPX_INVALID_FPX_HANDLE;
  
  if(theImage){
    float m11, m12, x0, m21, m22, y0;
    theImage->GetPosition(&x0, &y0, &m11, &m12, &m21, &m22);
    
    mat->a11 = m11;   mat->a12 = m12;   mat->a13 = 0;   mat->a14 = x0;
    mat->a21 = m21;   mat->a22 = m22;   mat->a23 = 0;   mat->a24 = y0;
    mat->a31 = 0;     mat->a32 = 0;     mat->a33 = 1;   mat->a34 = 0;
    mat->a41 = 0;     mat->a42 = 0;     mat->a43 = 0;   mat->a44 = 1;
    status = FPX_OK;
  }
  return status;  
}

FPXStatus FPX_SetImageInWorldAffineMatrix (
      FPXImageInWorld*    theImage,
      FPXAffineMatrix*  mat)
{
  FPXStatus status = FPX_INVALID_FPX_HANDLE;
  if(theImage){
      theImage->SetPosition(mat->a14, mat->a24, mat->a11, mat->a12, mat->a21, mat->a22);
     status = FPX_OK;
  }
  return status; 
}

// Formatted output tools :

FPXStatus FPX_SetPageSetup (
              FPXImageHandle* theFPX, 
              FPXPage**       thePage, 
              long            width, 
              long            height,
              float           rotation,
              FPXColorspace   backgroundColorspace,
              FPXBackground   backgroundColor)
{
  FPXStatus status = FPX_OK;
  
  if (!theFPX)
    status = FPX_INVALID_FPX_HANDLE;
  else {
    *thePage = new PageImage (theFPX, width, height, rotation);
    if (!*thePage)
      status = FPX_SEVER_INIT_ERROR;
    else
      FPX_SetViewBackgroundColor(backgroundColorspace,backgroundColor);
  }
  
  return status;
}

// Close Page :
FPXStatus FPX_ClosePage ( FPXPage* thePage)

{
  if (thePage == NULL) {
    return FPX_INVALID_FPX_HANDLE;
  } else {
    delete thePage;
    return FPX_OK;
  }
}
 
FPXStatus FPX_ReadPage (
              FPXPage*      thePage, 
              FPXImageDesc* renderingBuffer)
{
  FPXStatus status = FPX_OK;
  if (!thePage)
    status = FPX_INVALID_FPX_HANDLE;
  else {
    long width, height;
    thePage->GetPixelSize (&width, &height);
    FPXBufferDesc image(renderingBuffer,width,height);
    if (image.Get32BitsBuffer() == NULL) {
      return FPX_OBJECT_CREATION_FAILED;
    }
    if (image.GetBaselineColorSpace() == NON_AUTHORIZED_SPACE)
      status = FPX_INVALID_IMAGE_DESC;
    else {
      GtheSystemToolkit->SetUsedColorSpace(image.GetBaselineColorSpace());
      switch (thePage->ReadPage (image.Get32BitsBuffer())) {
        case -2 :
          status = FPX_FILE_READ_ERROR;
          break;
        case -3 :
          status = FPX_LOW_MEMORY_ERROR;
          break;
        case 0:
          image.UpdateDescriptor();
          break;
      default:
        {
        }
      }
    }
  }
  return status;
}

FPXStatus FPX_ReadPageLine (
              FPXPage*      thePage, 
              long          lineNumber, 
              FPXImageDesc* renderingBuffer)
{
  FPXStatus status = FPX_OK;
  if (!thePage)
    status = FPX_INVALID_FPX_HANDLE;
  else {
    long width, height;
    thePage->GetPixelSize (&width, &height);
    FPXBufferDesc line(renderingBuffer,width,1);
    if (line.Get32BitsBuffer() == NULL) {
      return FPX_OBJECT_CREATION_FAILED;
    }
    if (line.GetBaselineColorSpace() == NON_AUTHORIZED_SPACE)
      status = FPX_INVALID_IMAGE_DESC;
    else {
      GtheSystemToolkit->SetUsedColorSpace(line.GetBaselineColorSpace());
      switch (thePage->ReadPageLine (lineNumber, line.Get32BitsBuffer())) {
        case -2 :
          status = FPX_FILE_READ_ERROR;
          break;
        case -3 :
          status = FPX_LOW_MEMORY_ERROR;
          break;
        case 0:
          line.UpdateDescriptor();
          break;
      default:
        {
        }
      }
    }
  }
  return status;
}



//  - EOF ----------------------------------------------------------------------
