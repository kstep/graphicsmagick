//  ------------------------------------------------------------------------------------------------
//
//                    IMAGE VIEW TOOLKIT
//
//  This source code is the property of FITS Imaging inc.
//
//  MODULE    : AdvancedIVUE
//  LANGUAGE  : C++
//  AUTHOR    : Philippe BOSSUT
//  DATE    : Mardi 23 Novembre 1993
//  DESCRIPTION : 
//  COMMENT   :
//      SCCSID          : @(#)viewimg.cpp 1.4 12:48:05 01 Jul 1997
//  ------------------------------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ------------------------------------------------------------------------------------------------
  #include "viewimg.h"
//  ------------------------------------------------------------------------------------------------

//  Includes
//  --------

#ifndef Debug_h
  #include  "debug.h"
#endif
#ifndef Numbers_h
  #include  "numbers.h"
#endif

#ifndef PRIImage_h
  #include "priimage.h"
#endif

#ifndef ColorTwist_h
  #include "coltwist.h"
#endif

#ifndef FPXImageView_h
  #include  "fpximgvw.h"
#endif

#ifndef IVUEToolkit_h
  #include  "ri_lib.h"
#endif

#ifndef FileFlashPixIO_h
  #include  "f_fpxio.h"
#endif

//  Constants
//  ---------

//  Variables
//  ---------

//  ------------------------------------------------------------------------------------------------
#ifdef macintosh
  #pragma segment AdvancedIVUE
#endif
//  ------------------------------------------------------------------------------------------------

//  ------------------------------------------------------------------------------------------------
//  Internal Functions
//  ------------------------------------------------------------------------------------------------


//  ------------------------------------------------------------------------------------------------
//  Methods of the ViewWorld class
//  ------------------------------------------------------------------------------------------------


ViewWorld::ViewWorld (float width, float height, Pixel color) : state ((float)0.0, (float)0.0, width, height)
{
  // Init World size
  this->height    = Toolkit_ConvertToMm(height);
  this->width     = Toolkit_ConvertToMm(width);
  
  // Set the color of the System background
  GtheSystemToolkit->SetBackgroundColor (color,SPACE_32_BITS_RGB);
  
  // Init ViewImage list
  first       = NULL;
  last        = NULL;
  current       = NULL;
}


ViewWorld::~ViewWorld ()
{
  ViewImage*  next;
  
  // Delete all ViewImage within World
  current = first;
  while (current) {
    next = current->next;
    delete current;
    current = next;
  }
}


FPXStatus ViewWorld::AddImage (ref_FicNom refNom)
{
  register FPXStatus status = FPX_OK;
  ViewImage* image;

  // Open Image file (among other things–)
  if ((image = new PFlashPixImageView (refNom,NULL)) == NULL) {
    status = FPX_FILE_NOT_FOUND;    // Best guess as to why this failed
    goto end;
  }
  // Test if Image file is successfully opened, exit if not
  if (image->FileStatus() != 0) {
    delete image;
    image = NULL;
    status = FPX_FILE_NOT_OPEN_ERROR;
    goto end;
  }
  
  // Insert image in list
  if (!first)           // if list empty, set first element in list
    first = image;
  else {              // else add image at the end of the list
    image->previous = last;
    last->next = image;
  }
  last  = image;        // update last & current pointers
  current = image;
  
  {
  // Update state and modification rectangle
  PositionMv p0, p1;
  image->GetOutlineRectangle (&p0, &p1);
  state.Increment(p0,p1);
  }

end:
  return (status);
}


FPXStatus ViewWorld::AddImage (PRIImage* theImage)
{
  register FPXStatus status = FPX_OK;
  ViewImage* image;

  // Open image file (among other things–)
  if ((image = new ViewImage (theImage)) == NULL) {
    status = FPX_FILE_NOT_FOUND;    // Best guess as to why this failed
    goto end;
  }
  
  // Test if the image file is successfully opened, exit if not
  if (image->FileStatus() != 0) {
    delete image;
    image = NULL;
    status = FPX_FILE_NOT_OPEN_ERROR;
    goto end;
  }
  
  // Insert image in list
  if (!first)           // if list empty, set first element in list
    first = image;
  else {              // else add image at the end of the list
    image->previous = last;
    last->next = image;
  }
  last  = image;        // update last & current pointers
  current = image;
  
  {
  // Update state and modification rectangle
  PositionMv p0, p1;
  image->GetOutlineRectangle (&p0, &p1);
  state.Increment(p0,p1);
  }

end:
  return (status);
}


FPXStatus ViewWorld::AddImage (ViewImage* image)
{
  register FPXStatus status = FPX_OK;

  // Test if the image file is successfully opened, exit if not
  if (image->FileStatus() != 0) {
    status = FPX_FILE_NOT_OPEN_ERROR;
    goto end;
  }
  
  // Insert image in list
  if (!first)           // if list empty, set first element in list
    first = image;
  else {              // else add image at the end of the list
    image->previous = last;
    last->next = image;
  }
  last  = image;        // update last & current pointers
  current = image;
  
  {
  // Update state and modification rectangle
  PositionMv p0, p1;
  image->GetOutlineRectangle (&p0, &p1);
  state.Increment(p0,p1);
  }

end:
  return (status);
}


FPXStatus ViewWorld::DeleteImage (ViewImage* theImage, Boolean doNotCloseFile)
{
  register FPXStatus status = FPX_OK;
  ViewImage* image;

  // Error if list empty
  if (!first) {
    status = FPX_FILE_NOT_OPEN_ERROR;
    goto end;
  }
  
  // search theImage in list
  image = first;          // Init loop
  while (image != last) {
    if (image == theImage)    // Exit from loop if theImage found
      break;
    image = image->next;
  }
  if (image != theImage) {      // Error if theImage not found
    status = FPX_ERROR;
    goto end;
  }
  
  // delete image from list : don't forget to set current to a valid value if necessary–
  if ((image == first) && (image == last)) {  // if list contain a single image
    first   = NULL;
    last  = NULL;
    current = NULL;
  } else if (image == first) {        // if image is the first element of the list
    first = image->next;
    if (current == image)
      current = first;
  } else if (image == last) {         // if image is the last element of the list
    last = image->previous;
    if (current == image)
      current = last;
  } else {                  // if image in the middle of the list
    if (current == image)
      current = image->next;
  }
  
  {
  // Update state and modification rectangle
  PositionMv p0, p1;
  image->GetOutlineRectangle (&p0, &p1);
  state.Increment(p0,p1);
  }
  
  // delete the ViewImage itself and update pointers in list
  if (doNotCloseFile) {
    PRIImage* imageFile = image->GetImage();
    ((PHierarchicalImage*)(imageFile))->DoNotCloseFileWhenDelete();
  }
  delete image;
  
end:
  return (status);
}


long ViewWorld::First (ViewImage** theImage)
{
  register long status = 0;
  
  *theImage = first;  // Get first element of the list
  current = first;  // Set current pointer
  if (!first)     // Warning if list empty
    status = 1;
    
  return (status);
}


long ViewWorld::Last (ViewImage** theImage)
{
  register long status = 0;
  
  *theImage = last; // Get last element of the list
  current = last;   // Set current pointer
  if (!last)      // Warning if list empty
    status = 1;
    
  return (status);
}


long ViewWorld::Next (ViewImage** theImage)
{
  register long status = 0;
  
  if (!first)           // Warning if list empty
    status = 1;
  else {
    *theImage = current->next;  // get next element
    if (!*theImage)       // Warning if bottom of the list reached
      status = 2;
    else            // set current pointer
      current = *theImage;
  }
    
  return (status);
}


long ViewWorld::Previous (ViewImage** theImage)
{
  register long status = 0;
  
  if (!first)             // Warning if list empty
    status = 1;
  else {
    *theImage = current->previous;  // get previous element
    if (!*theImage)         // Warning if top of the list reached
      status = 2;
    else              // set current pointer
      current = *theImage;
  }
    
  return (status);
}


long ViewWorld::Current (ViewImage** theImage)
{
  register long status = 0;
  
  if (!current)       // Warning if list empty
    status = 1;
  else
    *theImage = current;    // get current element
    
  return (status);
}


//  ------------------------------------------------------------------------------------------------
//  Methods of the ViewImage class
//  ------------------------------------------------------------------------------------------------

ViewImage::ViewImage (ref_FicNom /*refName*/) : previous(0), next(0), state((float)0.0, (float)0.0, (float)0.0, (float)0.0)
{
  dirtyCount = 0;
  doNotDelete = FALSE;

  // Init Viewing parameters
  image = NULL;
  InitViewParameters();
}


ViewImage::ViewImage () : previous(0), next(0), state((float)0.0, (float)0.0, (float)0.0, (float)0.0)
{
  dirtyCount = 0;
  doNotDelete = FALSE;

  // Init Viewing parameters
  image = NULL;
  InitViewParameters();
}


ViewImage::ViewImage (PRIImage* theImage) : previous(0), next(0), state((float)0.0, (float)0.0, (float)0.0, (float)0.0)
{
  dirtyCount = 0;
  doNotDelete = TRUE;

  // Store the Image pointer
  image = theImage;
  
  // Test if Image file is successfully opened
  if (image->Status() != 0)
    image = NULL;

  // Init Viewing parameters
  InitViewParameters();

  // Copy FPX image viewing parameters
  if (image && image->imageParam) {
    hasColorTwist     = image->imageParam->HasColorTwist();
    if (hasColorTwist) {
      colorTwist = new PColorTwist();
      if (colorTwist)
        image->imageParam->GetColorTwist(colorTwist);
      else
        hasColorTwist = FALSE;
    }
    
    hasContrastValue  = image->imageParam->HasContrastValue();
    image->imageParam->GetContrast(&contrastValue);
    
    hasFilteringValue = image->imageParam->HasFilteringValue();
    image->imageParam->GetFiltering(&filteringValue);
    
    dirtyCount = image->imageParam->GetDirtyCount ();
    
    originHeight = image->imageParam->originHeight;
    originWidth  = image->imageParam->originWidth;
    height     = image->imageParam->height;
    width      = image->imageParam->width;
    cropX0     = image->imageParam->cropX0;
    cropY0     = image->imageParam->cropY0;
    cropX1     = image->imageParam->cropX1;
    cropY1     = image->imageParam->cropY1;
    position   = image->imageParam->position;
    resizeFinalDimensions = image->imageParam->resizeFinalDimensions;
    image->imageParam->GetFinalDimensions(&finalWidth, &finalHeight);
  }
}



void ViewImage::InitViewParameters()
{
  if (image) {
    // Init image size within World according to scanned size
    long pix_height, pix_width;
    float resol;
    image->GetDimensions (&pix_width, &pix_height);
    image->GetResolution (&resol);
    originHeight = (float)(pix_height) / resol;
    originWidth  = (float)(pix_width)  / resol;
    height = originHeight;
    width  = originWidth;
    cropX0 = (float)0;  cropY0 = (float)0;
    cropX1 = width; cropY1 = height;
    resizeFinalDimensions = TRUE;
    finalHeight = height;
    finalWidth  = width;
    
    // Store the back pointer in image
    if(!image->imageParam) 
      image->SetImageParameter(this);
  }
  normalizationRatio = (float)1.0;
  contrastValue = (float)1.0;
  filteringValue  = (float)0.0;
  colorTwist    = NULL;
  // Remark: position are automatically set to identity when created -> no need to reinit them

  // By default, none of these values has to be taken into account
  hasColorTwist     = FALSE;
  hasContrastValue  = FALSE;
  hasFilteringValue = FALSE;
}


ViewImage::~ViewImage ()
{
  // LS 06/27/96: remove the pointer to myself inside "image"
  if ( (image != NULL) && (image->imageParam == this) )
    image->SetImageParameter(NULL);
  
  // Delete Image if present
  if (image && !doNotDelete)
    delete image;

  // Delete ColorTwist
  if (colorTwist)
    delete colorTwist;
  
  // Update pointers up and down in the list
  if (next)
    next->previous = previous;
  if (previous)
    previous->next = next;
}
        

FPXStatus ViewImage::FileStatus ()
{
  register FPXStatus status = FPX_OK;
  
  // test if image exist (has been opened)
  if (image == NULL)
    status = FPX_FILE_NOT_OPEN_ERROR;
    
  return(status);
}
    
// Position setting tools : 

FPXStatus ViewImage::UseAlphaChannel (Boolean useAlpha)
{
  register FPXStatus status = FPX_OK;
  
  // test if image exist (has been opened)  
  if (!(status = FileStatus())) {
  
    status = image->UseAlphaChannel(useAlpha);
    
    // Compute rectangle of the image
    PositionMv  p0, p1;
    GetOutlineRectangle (&p0, &p1);
    RectangleMv rectangle (p0, p1);

    // Update state and modification rectangle
    state.Increment(rectangle);
  }
  return status;
}


FPXStatus ViewImage::InvertAlphaChannel (Boolean inverseAlpha)
{
  register FPXStatus status = FPX_OK;
  
  // test if image exist (has been opened)  
  if (!(status = FileStatus())) {
  
    status = image->InvertAlphaChannel(inverseAlpha);
    
    // Compute rectangle of the image
    PositionMv  p0, p1;
    GetOutlineRectangle (&p0, &p1);
    RectangleMv rectangle (p0, p1);

    // Update state and modification rectangle
    state.Increment(rectangle);
  }
  return status;
}


FPXStatus ViewImage::SetImageSize (float theWidth, float theHeight)
{
  if ((theHeight <= 0.0001) || (theWidth <= 0.0001))  // Error if size doesn't make sense
    return FPX_ERROR;
  else {                        // Rescale the image to fit the given size
  
    // Compute the current size of the image
    float x0, y0, x1, y1;
    GetOutlineRectangle (&x0, &y0, &x1, &y1);
    float previousWidth  = x1 - x0;
    float previousHeight = y1 - y0;
    
    // Rescale the image
    GetOrigin (&x0, &y0);             // CAUTION : x0, y0 are in unit but Scale() use unit–
    Scale (x0, y0, theWidth/previousWidth, theHeight/previousHeight);
  }
  return FPX_OK;
}


void ViewImage::SetPosition (TransfoPerspective& position)
{
  // Update transformation matrix with given values without modification
  this->position = position;
}


void ViewImage::SetPosition (float x0, float y0, float m11, float m12, float m21, float m22)
{
  TransfoPerspective trans (m11, m21, m12, m22, (float)0.0, (float)0.0);
  
  // Update transformation matrix with given values without modification
  position = trans;
  Translate (x0, y0);
}


void ViewImage::GetPosition (float* x0, float* y0, float* m11, float* m12, float* m21, float* m22)
{
  TransfoPerspective uvToxy (position);
  
  uvToxy.GetAffineMatrix(m11,m21,m12,m22,x0,y0);
}


void ViewImage::ApplyTransform (TransfoPerspective& trans)
{
  PositionMv  p0, p1;

  // Compute initial rectangle of the image
  GetOutlineRectangle (&p0, &p1);
  RectangleMv sourceRectangle (p0, p1);

  // Update transformation matrix with given values without modification
  position *= trans;
  
  // Compute final rectangle of the image
  GetOutlineRectangle (&p0, &p1);
  RectangleMv destinationRectangle (p0, p1);
  
  // Update final dimensions if required
  if (resizeFinalDimensions) {
    finalWidth  = p1.h;
    finalHeight = p1.v;
  }

  // Update state and modification rectangle
  state.Increment(sourceRectangle + destinationRectangle);
}


void ViewImage::ApplyTransform (float x0, float y0, float m11, float m12, float m21, float m22)
{
  // Create the transformation matrix
  TransfoPerspective trans (m11, m21, m12, m22, -x0, -y0);
  ApplyTransform(trans);
}


void ViewImage::Translate (float dx, float dy)
{
  PositionMv  p0, p1;

  dx = Toolkit_ConvertToMm(dx);
  dy = Toolkit_ConvertToMm(dy);
  
  // Compute initial rectangle of the image
  GetOutlineRectangle (&p0, &p1);
  RectangleMv sourceRectangle (p0, p1);

  // Translate origin of the image
  position.Translate(dx,dy);

  // Compute final rectangle of the image
  GetOutlineRectangle (&p0, &p1);
  RectangleMv destinationRectangle (p0, p1);

  // Update final dimensions if required
  if (resizeFinalDimensions) {
    finalWidth  = p1.h;
    finalHeight = p1.v;
  }

  // Update state and modification rectangle
  state.Increment(sourceRectangle + destinationRectangle);
}


void ViewImage::Rotate (float x0, float y0, float theta)
{
  PositionMv  p0, p1;

  x0 = Toolkit_ConvertToMm(x0);
  y0 = Toolkit_ConvertToMm(y0);
  
  // Compute initial rectangle of the image
  GetOutlineRectangle (&p0, &p1);
  RectangleMv sourceRectangle (p0, p1);

  // Rotate the image of an angle theta
  // (x0,y0) is the center of the rotation expressed in World coordinates
  // theta is in degree and in reverse trigonometric sense -> conversion in radian
  position.Rotate (x0, y0, -theta*(float)PI/(float)180.0);

  // Compute final rectangle of the image
  GetOutlineRectangle (&p0, &p1);
  RectangleMv destinationRectangle (p0, p1);

  // Update final dimensions if required
  if (resizeFinalDimensions) {
    finalWidth  = p1.h;
    finalHeight = p1.v;
  }

  // Update state and modification rectangle
  state.Increment(sourceRectangle + destinationRectangle);
}


void ViewImage::Scale (float x0, float y0, float sx, float sy)
{
  PositionMv  p0, p1;

  x0 = Toolkit_ConvertToMm(x0);
  y0 = Toolkit_ConvertToMm(y0);
  
  // Compute initial rectangle of the image
  GetOutlineRectangle (&p0, &p1);
  RectangleMv sourceRectangle (p0, p1);

  // Scale the image along x and y axes
  // (x0,y0) is the center of the scaling effect expressed in World coordinates
  position.Scale (x0, y0, sx, sy);

  // Compute final rectangle of the image
  GetOutlineRectangle (&p0, &p1);
  RectangleMv destinationRectangle (p0, p1);

  // Update final dimensions if required
  if (resizeFinalDimensions) {
    finalWidth  = p1.h;
    finalHeight = p1.v;
  }

  // Update state and modification rectangle
  state.Increment(sourceRectangle + destinationRectangle);
}


void ViewImage::ShearHorizontal (float x0, float y0, float sh)
{
  PositionMv  p0, p1;

  x0 = Toolkit_ConvertToMm(x0);
  y0 = Toolkit_ConvertToMm(y0);
  
  // Compute initial rectangle of the image
  GetOutlineRectangle (&p0, &p1);
  RectangleMv sourceRectangle (p0, p1);

  // Skew the image along x axe
  // (x0,y0) is the center of the skewing effect expressed in World coordinates
  position.ShearHorizontal (x0, y0, sh);

  // Compute final rectangle of the image
  GetOutlineRectangle (&p0, &p1);
  RectangleMv destinationRectangle (p0, p1);

  // Update final dimensions if required
  if (resizeFinalDimensions) {
    finalWidth  = p1.h;
    finalHeight = p1.v;
  }

  // Update state and modification rectangle
  state.Increment(sourceRectangle + destinationRectangle);
}


void ViewImage::ShearVertical (float x0, float y0, float sh)
{
  PositionMv  p0, p1;

  x0 = Toolkit_ConvertToMm(x0);
  y0 = Toolkit_ConvertToMm(y0);
  
  // Compute initial rectangle of the image
  GetOutlineRectangle (&p0, &p1);
  RectangleMv sourceRectangle (p0, p1);

  // Skew the image along y axe
  // (x0,y0) is the center of the skewing effect expressed in World coordinates
  position.ShearVertical (x0, y0, sh);

  // Compute final rectangle of the image
  GetOutlineRectangle (&p0, &p1);
  RectangleMv destinationRectangle (p0, p1);

  // Update final dimensions if required
  if (resizeFinalDimensions) {
    finalWidth  = p1.h;
    finalHeight = p1.v;
  }

  // Update state and modification rectangle
  state.Increment(sourceRectangle + destinationRectangle);
}


void ViewImage::FlipHorizontal (float x0, float y0)
{
  PositionMv  p0, p1;

  x0 = Toolkit_ConvertToMm(x0);
  y0 = Toolkit_ConvertToMm(y0);
  
  // Compute initial rectangle of the image
  GetOutlineRectangle (&p0, &p1);
  RectangleMv sourceRectangle (p0, p1);

  // Flip the image along a vertical line
  // (x0,y0) is the center of the flip effect expressed in World coordinates
  position.FlipHorizontal (x0,y0);

  // Compute final rectangle of the image
  GetOutlineRectangle (&p0, &p1);
  RectangleMv destinationRectangle (p0, p1);

  // Update final dimensions if required
  if (resizeFinalDimensions) {
    finalWidth  = p1.h;
    finalHeight = p1.v;
  }

  // Update state and modification rectangle
  state.Increment(sourceRectangle + destinationRectangle);
}


void ViewImage::FlipVertical (float x0, float y0)
{
  PositionMv  p0, p1;

  x0 = Toolkit_ConvertToMm(x0);
  y0 = Toolkit_ConvertToMm(y0);
  
  // Compute initial rectangle of the image
  GetOutlineRectangle (&p0, &p1);
  RectangleMv sourceRectangle (p0, p1);

  // Flip the image along an horizontal line
  // (x0,y0) is the center of the flip effect expressed in World coordinates
  position.FlipVertical (x0,y0);

  // Compute final rectangle of the image
  GetOutlineRectangle (&p0, &p1);
  RectangleMv destinationRectangle (p0, p1);

  // Update final dimensions if required
  if (resizeFinalDimensions) {
    finalWidth  = p1.h;
    finalHeight = p1.v;
  }

  // Update state and modification rectangle
  state.Increment(sourceRectangle + destinationRectangle);
}


void ViewImage::SetContrast (float theContrastValue)
{
  contrastValue = theContrastValue;
  hasContrastValue = (((contrastValue > 1.00001) || (contrastValue < 0.99999)) ? TRUE : FALSE);
  dirtyCount++;
}


void ViewImage::SetColorTwist (PColorTwist* theColorTwist)
{
  if (theColorTwist) {
    if (colorTwist)
      delete colorTwist;
    colorTwist = NULL;
    hasColorTwist = (theColorTwist->IsIdentity() ? FALSE : TRUE);
    if (hasColorTwist) {
      colorTwist  = new PColorTwist();
      if (colorTwist)
        *colorTwist = *theColorTwist;
      else
        hasColorTwist = FALSE; 
    }
    dirtyCount++;
  }
}


void ViewImage::SetFiltering (float theFilteringValue)
{
  filteringValue = theFilteringValue;
  hasFilteringValue = (((filteringValue > 0.00001) || (filteringValue < -0.00001)) ? TRUE : FALSE);
  dirtyCount++;
}


long ViewImage::SetFinalDimensions (float theFinalWidth, float theFinalHeight)
{
  finalHeight = theFinalHeight;
  finalWidth  = theFinalWidth;
  return 0;
}


void ViewImage::GetContrast (float* theContrastValue)
{
  *theContrastValue = contrastValue;
}


void ViewImage::GetColorTwist (PColorTwist* theColorTwist)
{
  if (colorTwist)
    *theColorTwist = *colorTwist;
  else {
    PColorTwist identity;
    *theColorTwist = identity;
  }
}


void ViewImage::GetFiltering (float* theFilteringValue)
{
  *theFilteringValue = filteringValue;
}


void ViewImage::GetFinalDimensions (float* theFinalWidth, float* theFinalHeight)
{
  *theFinalHeight = finalHeight;
  *theFinalWidth  = finalWidth;
}


long ViewImage::GetDirtyCount ()
{
  return dirtyCount;
}


FPXStatus ViewImage::SetImageCrop(float x0, float y0, float x1, float y1)
{
  // Test bounds to ensure rect is within the image
  if (x0 < 0.0)
    x0 = 0.0;
  if (x1 > originWidth)
    x1 =  originWidth;
  
  if (y0 < 0.0)
    y0 = 0.0;
  if (y1 > originHeight)
    y1 =  originHeight;
  
  if ((x0 >= x1) || (y0 >= y1)) 
    return FPX_BAD_COORDINATES;

  // Reset the cropping before applying the new one
  ResetImageCrop();

  // Get the crop rect in dot per mm in image space
  PositionMv p0 (x0, y0);
  PositionMv p1 (x1, y1);


  // Set the cropping rectangle by converting image coords into pixel
  // coords using the current resolution
  float resol;
  image->GetResolution (&resol);
  image->SetCropRectangle (FLOAT_TO_LONG(p0.h*resol), FLOAT_TO_LONG(p0.v*resol), 
              FLOAT_TO_LONG(p1.h*resol), FLOAT_TO_LONG(p1.v*resol));

  // Update width and height according to the cropping definition
  long  pix_height, pix_width;
  image->GetDimensions (&pix_width, &pix_height);
  height = (float)(pix_height) / resol;
  width = (float)(pix_width)  / resol;

  // Update final dimensions if required
  PositionMv pt0, pt1;
  if (resizeFinalDimensions) {
    GetOutlineRectangle (&pt0, &pt1);
    finalWidth  = pt1.h;
    finalHeight = pt1.v;
  }
  
  // Save new crop coordinates
  cropX0 = x0;
  cropX1 = x1;
  cropY0 = y0;
  cropY1 = y1;

  // Update state and modification rectangle
  RectangleMv rectangle (pt0, pt1);
  state.Increment(rectangle); 

  return FPX_OK;
}


void ViewImage::ResetImageCrop()
{
  long  i0, j0, i1, j1;
  long  x0, y0, x1, y1;
  float resol;
  TransfoPerspective uvToxy (position);

  // Inverse position matrix (position is the xy to uv transformation)
  uvToxy.Inverse();

  // Get the crop rectangle before reset
  image->GetCropRectangle (&i0, &j0, &i1, &j1);

  // Reset crop rectangle
  image->ResetCropRectangle ();
  
  // Get the crop rectangle after reset
  image->GetCropRectangle (&x0, &y0, &x1, &y1);

  // Update width and height according to the new cropping definition
  image->GetResolution (&resol);
  width = (float)(x1 - x0) / resol;
  height  = (float)(y1 - y0) / resol;
  cropX0 = (float)0;  cropY0 = (float)0;
  cropX1 = width; cropY1 = height;
  
  // Compute rectangle of the image
  PositionMv  p2, p3;
  GetOutlineRectangle (&p2, &p3);
  RectangleMv rectangle (p2, p3);

  // Update final dimensions if required
  if (resizeFinalDimensions) {
    finalWidth  = p3.h;
    finalHeight = p3.v;
  }
  
  // Update state and modification rectangle
  state.Increment(rectangle);
}


// Get geometric information tools :

long ViewImage::GetOutlineParallelogram (float* x0, float* y0, float* x1, float* y1,
                     float* x2, float* y2, float* x3, float* y3)
{
  PositionMv p0, p1, p2, p3;

  // get positions of the 4 points
  GetOutlineParallelogram (&p0, &p1, &p2, &p3);
  
  // update float values according to point positions
  *x0 = Toolkit_ConvertToUnit(p0.h);
  *y0 = Toolkit_ConvertToUnit(p0.v);

  *x1 = Toolkit_ConvertToUnit(p1.h);
  *y1 = Toolkit_ConvertToUnit(p1.v);

  *x2 = Toolkit_ConvertToUnit(p2.h);
  *y2 = Toolkit_ConvertToUnit(p2.v);

  *x3 = Toolkit_ConvertToUnit(p3.h);
  *y3 = Toolkit_ConvertToUnit(p3.v);
  
  return 0;
}


void ViewImage::GetOutlineParallelogram (PositionMv* p0, PositionMv* p1, PositionMv* p2, PositionMv* p3)
{
  TransfoPerspective  uvToxy (position);
  PositionMv      pointUV;


  // Inverse position matrix (position is the xy to uv transformation)
  uvToxy.Inverse();

  // Set first corner of image and transform
  pointUV.h = cropX0;
  pointUV.v = cropY0;
  *p0 = uvToxy * pointUV;


  // Set second corner of image and transform
  pointUV.h = cropX1;
  pointUV.v = cropY0;
  *p1 = uvToxy * pointUV;


  // Set third corner of image and transform
  pointUV.h = cropX0;
  pointUV.v = cropY1;
  *p2 = uvToxy * pointUV;


  // Set fourth corner of image and transform
  pointUV.h = cropX1;
  pointUV.v = cropY1;
  *p3 = uvToxy * pointUV;
}


void ViewImage::GetOutlineRectangle (float* x0, float* y0, float* x1, float* y1)
{
  PositionMv p0, p1;

  // get positions of the 2 points
  GetOutlineRectangle (&p0, &p1);
  
  // update float values according to point positions
  *x0 = Toolkit_ConvertToUnit(p0.h);
  *y0 = Toolkit_ConvertToUnit(p0.v);

  *x1 = Toolkit_ConvertToUnit(p1.h);
  *y1 = Toolkit_ConvertToUnit(p1.v);
}


void ViewImage::GetOutlineRectangle (PositionMv* p0, PositionMv* p1)
{
  PositionMv p[4];

  // get parallelogram outline positions
  GetOutlineParallelogram (&p[0], &p[1], &p[2], &p[3]);

  // init min and max values with the first point
  *p0 = p[0];
  *p1 = p[0];
  
  // loop to find min and max values in both direction
  for (short i = 1; i < 4; i++) {
    if (p[i].h < p0->h) p0->h = p[i].h;
    if (p[i].v < p0->v) p0->v = p[i].v;
    if (p[i].h > p1->h) p1->h = p[i].h;
    if (p[i].v > p1->v) p1->v = p[i].v;
  }
}


void ViewImage::GetOrigin (float* x0, float* y0)
{
  PositionMv p0;

  // get positions of the point
  GetOrigin (&p0);
  
  // update float values according to point positions
  *x0 = Toolkit_ConvertToUnit(p0.h);
  *y0 = Toolkit_ConvertToUnit(p0.v);
}


void ViewImage::GetOrigin (PositionMv* p0)
{
  PositionMv p[4];

  // get parallelogram outline positions
  GetOutlineParallelogram (&p[0], &p[1], &p[2], &p[3]);

  // origin is the first point
  *p0 = p[0];
}


// Read a rectangle of the transformed image

FPXStatus ViewImage::Read4x4Points (float x0, float y0, float x1, float y1, Pixel* bufPix)
{
  FPXStatus status = FPX_OK;

  // Save the background (for the post cropping)
  Pixel backGround[16];
  BlockMove (bufPix, backGround, 16*sizeof(Pixel));

  PositionMv topleft(x0,y0);
  PositionMv topright(x1,y0);
  PositionMv botleft(x0,y1);
  PositionMv botright(x1,y1);

  topleft  = position * topleft;
  topright = position * topright;
  botleft  = position * botleft;
  botright = position * botright;

  float resolution;
  image->GetResolution(&resolution);
  resolution *= (float)4096.0;

  long i0 = (FLOAT_TO_LONG)((topleft.h ) * resolution);
  long j0 = (FLOAT_TO_LONG)((topleft.v ) * resolution);
  long i1 = (FLOAT_TO_LONG)((topright.h) * resolution);
  long j1 = (FLOAT_TO_LONG)((topright.v) * resolution);
  long i2 = (FLOAT_TO_LONG)((botleft.h ) * resolution);
  long j2 = (FLOAT_TO_LONG)((botleft.v ) * resolution);
  long i3 = (FLOAT_TO_LONG)((botright.h) * resolution);
  long j3 = (FLOAT_TO_LONG)((botright.v) * resolution);

  // Read the sample
  status = image->Read4Points (i0, j0, i1, j1, i2, j2, i3, j3, bufPix, -1);

  // Crop the result image
  if ((x0 < 0.0) || (y0 < 0.0) || (x1 > finalWidth) || (y1 > finalHeight)) {
    float dx = (x1 - x0) / (float)4.0;
    float dy = (y1 - y0) / (float)4.0;
    float x, y;
    long* pt = (long*)(bufPix);
    long* bk = (long*)(backGround);
    y = y0; 
    for (long i = 0; i < 4; ++i) {
      x = x0;
      for (long j = 0; j < 4; ++j) {
        if ((x < 0.0) || (y < 0.0) || (x > finalWidth) || (y > finalHeight))
          *pt = *bk;
        x += dx;
        pt++; bk++;
      }
      y += dy;
    }
  }

  return status;
}


//  ------------------------------------------------------------------------------------------------
//  Methods of the ViewWindow class
//  ------------------------------------------------------------------------------------------------

ViewWindow::ViewWindow (ViewWorld* world, float x0, float y0, float width, float height, float resolution)
{
  // Update Window record
  this->world     = world;
  this->resolution  = resolution;
  this->x0      =   Toolkit_ConvertToMm(x0);
  this->y0      =   Toolkit_ConvertToMm(y0);

  // Compute Window size in pixels
  // CAUTION : WorldToWindow() use unit !
  WorldToWindow (x0 + width, y0 + height, &pix_width, &pix_height);
  // Convert back to units
  WindowToWorld (pix_width, pix_height, &width, &height);
  
  // Update Window record in mm
  this->width  = Toolkit_ConvertToMm(width  - x0);
  this->height = Toolkit_ConvertToMm(height - y0);
  
  // State flag and integral transformation chain initialization
  modifiedWindow    = TRUE;
  worldState      = 0;
  first       = NULL;
}


ViewWindow::~ViewWindow ()
{
  ViewTransfoState* next;
  ViewTransfoState* current;
  
  // Delete integral transformation chain
  current = first;
  while (current) {
    next = current->next;
    delete current;
    current = next;
  }
}


// Basic access tools :

FPXStatus ViewWindow::ReadSample (long i0, long j0, Pixel* sample, FPXBaselineColorSpace space)
{
  float     px0, py0, px1, py1;
  ViewImage *image;
  FPXStatus status = FPX_OK;
  Pixel     sampleTmp[SAMPLE_WIDTH*SAMPLE_WIDTH];
  
  // Compute Window position of each corner of the sample
  px0 = ((float)(i0)/resolution) + x0;
  py0 = ((float)(j0)/resolution) + y0;
  px1 = px0 + (float)(SAMPLE_WIDTH)/resolution;
  py1 = py0 + (float)(SAMPLE_WIDTH)/resolution;
  
  // Set the global color model used by the user and the sample
  GtheSystemToolkit->SetUsedColorSpace(space);
  GtheSystemToolkit->SetToBackground(sampleTmp,SAMPLE_WIDTH,SAMPLE_WIDTH);

  // Read sample for each image of the World
  world->First(&image);
  while (image) {

    // Set the image color space: supported only by FlashPix images for the moment...
    PFileFlashPixIO* FlashPixFile = (PFileFlashPixIO*)(image->GetImage());
    FlashPixFile->SetUsedColorSpace(space);

    // Read sample in file
    if ((status = image->Read4x4Points (px0, py0, px1, py1, sampleTmp)) != FPX_OK)
      return status;
    world->Next(&image);                // Next image
  }
  
  // Copy the sample read according to the interleaving and the active channel choosed by the user
  short plan = Toolkit_ActiveChannel();
  // Copy pixels to the tile
  if (plan == ActiveChannel_All) {
    // move all the buffer if all planes available to copy
    BlockMove(sampleTmp,sample,SAMPLE_WIDTH*SAMPLE_WIDTH*sizeof(Pixel));
  } else {
    if (GtheSystemToolkit->interleaving == Interleaving_Channel) {
      // Pixel by pixel if planes specified and there is only one channel in the output buffer
      register long j;
      register unsigned char *src, *dst;
      src = (unsigned char *)(sampleTmp) + plan; 
      dst = (unsigned char *)(sample);
      for (j=0; j<SAMPLE_WIDTH*SAMPLE_WIDTH;j++,dst++,src+=sizeof(Pixel))
        *dst = *src;
    } else {
      // Pixel by pixel if planes specified
      register long j;
      register unsigned char *src, *dst;
      src = (unsigned char *)(sampleTmp) + plan; 
      dst = (unsigned char *)(sample)  + plan;
      for (j=0; j<SAMPLE_WIDTH*SAMPLE_WIDTH;j++,src+=sizeof(Pixel),dst+=sizeof(Pixel))
        *dst = *src;
    }
  }
  if ((GtheSystemToolkit->interleaving != Interleaving_Channel) || (plan == ActiveChannel_All))
    if (Toolkit_Interleave (sample, SAMPLE_WIDTH, SAMPLE_WIDTH))
      status = FPX_MEMORY_ALLOCATION_FAILED;
  return status;
}


FPXStatus ViewWindow::Refresh (Pixel* map, FPXBaselineColorSpace space, long pixelWidth,
              long pixelHeight)
{
  long    i, j;
  Pixel     sample[SAMPLE_WIDTH*SAMPLE_WIDTH];
  FPXStatus status = FPX_OK;
  
  j = 0;
  while (j < pixelHeight) {                       // For each horizontal bar
    i = 0;
    if ( GtheSystemToolkit->fnctProgFunc)
       if (GtheSystemToolkit->fnctProgFunc( pixelHeight, j))
        return FPX_USER_ABORT;

    while ((i < pixelWidth) && (status == FPX_OK)) {
      status = ReadSample (i, j, sample, space);              // Read sample
      Toolkit_CopyInterleaved (map, pixelWidth, pixelHeight,
                   sample, SAMPLE_WIDTH, SAMPLE_WIDTH, i, j);
      i += SAMPLE_WIDTH;                          // Next sample
    }
    j += SAMPLE_WIDTH;                            // Next horizontal bar
  }
  return status;
}


// Modifying Window tools :

void ViewWindow::Translate (float dx, float dy)
{
  // translate Window origin
  x0 += Toolkit_ConvertToMm(dx);
  y0 += Toolkit_ConvertToMm(dy);
  
  // Update Window state
  modifiedWindow = TRUE;
}


void ViewWindow::Resize (float width, float height)
{
  // Compute Window size in pixels
  // CAUTION : WorldToWindow() use unit => conversion of x0 and y0 from mm to unit
  WorldToWindow (Toolkit_ConvertToUnit(x0) + width, Toolkit_ConvertToUnit(y0) + height, &pix_width, &pix_height);
  
  // Convert to a multiple of SAMPLE_WIDTH
  // CHG_FIX_RESIZE - rounding-up here causes RI to write beyond the end of the users
  //  buffer later. So don't recalculate pixel sizes here and just let the rendering
  //  code handle special cases where buffer is not a multiple of SAMPLE_WIDTH
  //pix_width  = (((pix_width -1)/SAMPLE_WIDTH) + 1) * SAMPLE_WIDTH;
  //pix_height = (((pix_height-1)/SAMPLE_WIDTH) + 1) * SAMPLE_WIDTH;
  
  // Convert back to unit
  WindowToWorld (pix_width, pix_height, &width, &height);
  
  // Set the new size in mm
  this->width  = Toolkit_ConvertToMm(width)  - x0;
  this->height = Toolkit_ConvertToMm(height) - y0;
  
  // Update Window state
  modifiedWindow = TRUE;
}


FPXStatus ViewWindow::Zoom (float zoomRatio)
{
  // Test parameter consistency
  if (zoomRatio <= 0.0)
    return FPX_ERROR;
    
  // update resolution according to zoomRatio
  resolution *= zoomRatio;
  
  // zoom is performed from the center of the Window, so, translate the origin
  x0 += width  * (zoomRatio - 1) / (2*zoomRatio);
  y0 += height * (zoomRatio - 1) / (2*zoomRatio);
  
  // size is changed in order to maintain the same amount of pixels in the Window
  width /= zoomRatio;
  height /= zoomRatio;
  
  // Note that pix_width and pix_height are not changed through zoom effect–
  // Update Window state
  modifiedWindow = TRUE;
  
  return FPX_OK;
}


// Get usefull information :

void ViewWindow::GetWindowDefinition (float* x0, float* y0, float* width, float* height, float* resolution)
{
  // read information from Window record
  *x0       = Toolkit_ConvertToUnit(this->x0);
  *y0       = Toolkit_ConvertToUnit(this->y0);
  *height     = Toolkit_ConvertToUnit(this->height);
  *width      = Toolkit_ConvertToUnit(this->width);
  *resolution   = Toolkit_ConvertToMm(this->resolution);
}


void ViewWindow::WindowToWorld (long i, long j, float* x, float* y)
{
  // Convert Window pixels coordinates into World coordinates
  *x = Toolkit_ConvertToUnit((float)(i)/resolution + x0);
  *y = Toolkit_ConvertToUnit((float)(j)/resolution + y0);
}


void ViewWindow::WorldToWindow (float x, float y, long* i, long* j)
{
  // Convert World coordinates into Window pixels coordinates
  *i = (long)((Toolkit_ConvertToMm(x)-x0) * resolution + 0.5);
  *j = (long)((Toolkit_ConvertToMm(y)-y0) * resolution + 0.5);
}


// Update integral transformation chain, allocate it if necessary
void ViewWindow::UpdateTransfoChain()
{
  if (modifiedWindow || (worldState!=world->GetState())) {// If World or Window state is different
    ViewImage*      image;
    ViewTransfoState* transfo;
    world->First(&image);               // Set image to be the first of the World
    if (!first) {
      first = new ViewTransfoState();         // Allocate first integral transformation
      if (first==NULL) {
        return;
      }
    }
    transfo = first;                  // Set transfo to be the first of the Window
    while (image) {                   // For every image of the World
      transfo->Update (x0, y0, resolution, image);  // Update transfo according to Window and image 
      world->Next(&image);              // Step to the next image
      if (!transfo->next) {
        transfo->next = new ViewTransfoState();   // Allocate next transfo if necessary
        if (first==NULL) {
          return;
        }
      }
      transfo = transfo->next;            // Step to the next transfo
    }
    modifiedWindow  = FALSE;              // Update Window state
    worldState    = world->GetState();        // Update World state
  }
}
  


//  ------------------------------------------------------------------------------------------------
//  Methods of the substructures
//  ------------------------------------------------------------------------------------------------

ViewWorldRect::~ViewWorldRect ()
{

}
  

// Compute transformation matrice according to Window and image parameters

void ViewTransfoState::Update (float x0, float y0, float resolution, ViewImage* image)
{
  long pix_height, pix_width;
  
  (*image)->GetDimensions (&pix_width, &pix_height);
  
  // Translation and scaling of pixel Window to World mm
  TransfoPerspective  windowposition ((float)1.0/resolution, (float)0.0, (float)0.0, (float)1.0/resolution, x0, y0);

  // Scaling of World mm to Image pixel
  TransfoPerspective  imagescaling ((float)(pix_width)/image->width, (float)0.0, (float)0.0, (float)(pix_height)/image->height, (float)0.0, (float)0.0);

  // Combinaison of the 3 matrices in float
  TransfoPerspective  scaledposition = imagescaling * (image->position * windowposition);

  // Convert matrice into integral transformation matrice
  pixelPosition = scaledposition;
  
  // Save image state
  imageState = image->GetState();
}


ViewState::ViewState(float x0, float y0, float x1, float y1) : largestRectangle (x0, y0, x1, y1)
{
  long i;
  
  // Initialize state and last modified rectangle
  stateNumber     = 1;
  lastModRectangle  = &modRectangles[0];
  
  // "modRectangles" array is a closed chain of "ViewStateNumber" members
  modRectangles[0].next   = &modRectangles[1];
  modRectangles[0].previous = &modRectangles[ViewStateNumber-1];
  for (i=1;i<(ViewStateNumber-1);i++) {
    modRectangles[i].next   = &modRectangles[i+1];
    modRectangles[i].previous = &modRectangles[i-1];
  }
  modRectangles[ViewStateNumber-1].next   = &modRectangles[0];
  modRectangles[ViewStateNumber-1].previous = &modRectangles[ViewStateNumber-2];
}


void ViewState::Increment()
{
  lastModRectangle = lastModRectangle->next;    // Set to next rectangle
  lastModRectangle->rectangle = largestRectangle; // Store rectangle
  stateNumber++;                  // Update state number  
}


void ViewState::Increment(const PositionMv& p0, const PositionMv& p1)
{
  RectangleMv modifiedArea (p0, p1);
  lastModRectangle = lastModRectangle->next;    // Set to next rectangle
  lastModRectangle->rectangle = modifiedArea;   // Store rectangle
  stateNumber++;                  // Update state number 
}


void ViewState::Increment(const RectangleMv& r)
{
  lastModRectangle = lastModRectangle->next;    // Set to next rectangle
  lastModRectangle->rectangle = r;        // Store rectangle
  stateNumber++;                  // Update state number
}


RectangleMv ViewState::GetRectangle (long fromState)
{
  RectangleMv result;
  
  if (fromState < stateNumber) {                  // If asking for an old state
    if ((fromState - stateNumber) > ViewStateNumber)      // If state to old–
      result = largestRectangle;                // –return largest rectangle
    else {
      ViewWorldRect* curModRectangle = lastModRectangle;
      result = curModRectangle->rectangle;          // make union of all previous
      while (++fromState != stateNumber) {          // state rectangles
        curModRectangle = curModRectangle->previous;
        result += curModRectangle->rectangle;
      }
    }
  }
  
  // Note : if "fromState" >= "stateNumber" the returned rectangle will be equal to 0.0 everywhere–
  
  return result;
}





