//  ----------------------------------------------------------------------------
//  MODULE    : PRIImage 
//  LANGUAGE  : C++ 
//  CREATOR   : Philippe BOSSUT 
//  CREAT. DATE : Tuesday, March 12, 1996 
//  DESCRIPTION :  
//  COMMENTS  : 
//  SCCSID      : @(#)priimage.cpp  1.1 11:46:42 18 Dec 1996
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
//  #define Memoire_Debug
//  ----------------------------------------------------------------------------

//  ----------------------------------------------------------------------------
  #include "priimage.h"
//  ----------------------------------------------------------------------------

//  Includes
//  --------

  #include  <stdio.h>
  #include  <stdlib.h>
  #include  <string.h>
#ifndef _WINDOWS
  #include  <strings.h>
#endif
  #include  <math.h>
#ifdef macintosh
  #include  <Errors.h>
#endif  

  
#ifndef Debug_h
  #include  "debug.h"
#endif
#ifndef Memoire_h
//include "b_memory.h"
#endif

#ifndef PResolutionLevel_h
  #include "pr_level.h"
#endif
#ifndef PTile_h
  #include "ptile.h"
#endif


#ifndef Numbers_h
  #include  "numbers.h"
#endif


//  Constants
//  ---------

  #define NB_MAX_ERREUR       5

//  Variables
//  ---------

  Boolean     PRIImage::readInterpolated = false; // Default is no antialiasing

//  ----------------------------------------------------------------------------

//  ----------------------------------------------------------------------------

//  ----------------------------------------------------------------------------
//  Internal Functions
//  ----------------------------------------------------------------------------

//  ----------------------------------------------------------------------------
//  Member Functions
//  ----------------------------------------------------------------------------

//  ----------------------------------------------------------------------------
//  Methods of the PRIImage class
//  ----------------------------------------------------------------------------


//  ----------------------------------------------------------------------------
// Constructor to be used when creating a new Image file
PRIImage::PRIImage (long width, long height, float resolution)
{
  Init ();                  // Init the object
  SetImageSize (width, height, resolution); // Set the high res size and related values
}


//  ----------------------------------------------------------------------------
// Constructor to be used when reading or writing onto an existing Image file
PRIImage::PRIImage ()

{
  Init ();    // Init the object
}


//  ----------------------------------------------------------------------------
// Close file and clean memory
PRIImage::~PRIImage ()
{
  ReleasePaths ();
}


//  ----------------------------------------------------------------------------
// Give default values to image information before starting reading or using them
void PRIImage::Init ()
{
  // PRIImage values
  height          = 0;
  width         = 0;
  resolution      = (float) (300.0/2.54);
  magicNumber     = 0;
  nbRefs        = 0;

  useAlphaChannel     = false;
  existAlphaChannel   = GtheSystemToolkit->existAlpha;
  inverseAlphaChannel = false;
  hotSpotX      = 0;
  hotSpotY      = 0;
  cropX0        = 0;
  cropY0        = 0;
  cropX1        = 0;
  cropY1        = 0;

  path        = NULL;
  nbPaths       = 0;
  clipPathNum     = 0;

  error       = FPX_OK;         // No error for nowÉ
  nbError       = 0;            // No counted error for nowÉ

  imageParam      = NULL; // Image viewing params set by ViewImage, not here
  applyParam      = TRUE; // By default, the viewing parameters are ALWAYS applied
}


// Set the image size and the related parameters
FPXStatus PRIImage::SetImageSize (long theWidth, long theHeight, float theResolution)
{
  resolution  = theResolution;

  height    = theHeight;
  width   = theWidth;

  cropX0    = 0;
  cropY0    = 0;
  cropX1    = width;
  cropY1    = height;
  hotSpotX  = width/2;
  hotSpotY  = height/2;
  return FPX_OK;
}


//  ----------------------------------------------------------------------------
FPXStatus PRIImage::GetFileName (FicNom& )
{
  return FPX_UNIMPLEMENTED_FUNCTION;
}
      

//  ----------------------------------------------------------------------------
void PRIImage::UpdateErrorCount()
{
  if (++nbError > NB_MAX_ERREUR)  // If too many errors...
    error = FPX_ERROR;      // ...then raise an error
}


//  ----------------------------------------------------------------------------
// Return the status of the image file
FPXStatus PRIImage::Status ()
{
  return(error);
}


//  ----------------------------------------------------------------------------
// Read a rectangle in the correct resolution level and fill 'pix' with it
FPXStatus PRIImage::ReadRectangle (long , long , long , long , Pixel* , long )
{
  return FPX_UNIMPLEMENTED_FUNCTION;
}


//  ----------------------------------------------------------------------------
FPXStatus PRIImage::Read4Points (long , long , long , long , long , long , long , long , Pixel* , long )
{
  return FPX_UNIMPLEMENTED_FUNCTION;
}


//  ----------------------------------------------------------------------------
// Send back the smallest image contained in a rectangle width * height.
// Read it on disk if necessary. 
// If decompressor is missing, send back NULL.
// CAUTION : bufferOut must be allocated and freed by calling routine
// CAUTION : we allocate a temporary buffer with pixelsPerLine*height pixels
FPXStatus PRIImage::ReadInARectangle (Pixel* , short , short , short , 
                  const CorrectLut* , Boolean , const CombinMat* )
{
  return FPX_UNIMPLEMENTED_FUNCTION;
}


//  ----------------------------------------------------------------------------
// Read the path from the Image file: this function has to be derived to really write the things in
// the file. The following code handle only the memory management part.
OSErr PRIImage::ReadPaths (data_Record** thePath, long* nbRecords, long* theClipPathNum)
{
  // Provide the cached path to the caller
  *nbRecords      = nbPaths;
  *thePath        = path;
  *theClipPathNum = clipPathNum;

  OSErr  err = noErr;
  if (path)
    // If there is a path already cached here, avoid to read again...
    err = noErr;
  else
    // Can't read here: see derived class...
    #ifdef macintosh
      err = userDataItemNotFound;
    #else
      err = -1;   // GDN - actual code found in Errors.h
    #endif
    
  return err;
}


//  ----------------------------------------------------------------------------
// Valid for all derived classes
OSErr PRIImage::ReleasePaths ()
{
  // Delete the path record if any
  if (path)
    delete [] path;
    
  // Reset all path related data
  path        = NULL;
  nbPaths     = 0;
  clipPathNum = 0;
  
  return noErr;
}


//  ----------------------------------------------------------------------------
FPXStatus PRIImage::UseAlphaChannel (Boolean useAlpha)
{
  if (existAlphaChannel)
    useAlphaChannel = useAlpha;
  return FPX_OK;
}


//  ----------------------------------------------------------------------------
// Set the Boolean existAlphaChannel and test if there is enougth space to use it
Boolean PRIImage::SetAlphaChannel(Boolean exist)
{
  existAlphaChannel = exist;
  return true;
}
  

//  ----------------------------------------------------------------------------
FPXStatus PRIImage::InverseAlpha ()
{
  return FPX_OK;
}


//  ----------------------------------------------------------------------------
FPXStatus PRIImage::InvertAlphaChannel (Boolean inverseAlpha)
{
  if (existAlphaChannel) {
    if (inverseAlphaChannel != inverseAlpha) {
      // Modify the flag
      inverseAlphaChannel = inverseAlpha;
      // Reinverse the values already cached
      InverseAlpha();
    }
  }
  return FPX_OK;
}


//  ----------------------------------------------------------------------------
FPXStatus PRIImage::GetCropRectangle (long* x0, long* y0, long* x1, long* y1)
{
  *x0 = cropX0; *y0 = cropY0; *x1 = cropX1; *y1 = cropY1; 
  return FPX_OK;  
}


//  ----------------------------------------------------------------------------
FPXStatus PRIImage::SetCropRectangle (long x0, long y0, long x1, long y1)
{
  // Check consistency : swap bounding values if necessary
  long t;
  if (x1 < x0) { t = x1; x1 = x0; x0 = t; }
  if (y1 < y0) { t = y1; y1 = y0; y0 = t; }

  // Limit crop rectangle to the true dimension of the image
  long width, height;
  if (GetTrueDimensions(&width, &height) != NOT_IMPLEMENTED) {
    if (x0 < 0)    x0 = 0;
    if (x1 > width)  x1 = width;
    if (y0 < 0)    y0 = 0;
    if (y1 > height) y1 = height;
  }

  // Set the crop rectangle
  cropX0 = x0; cropY0 = y0; cropX1 = x1; cropY1 = y1;
  return FPX_OK;  
}


//  ----------------------------------------------------------------------------
FPXStatus PRIImage::ResetCropRectangle ()
{
  cropX0 = 0; cropY0 = 0; cropX1 = width; cropY1 = height;
  return FPX_OK;  
}


//  ----------------------------------------------------------------------------
// Read a pixel of a sub image. Read it on disk if necessary. 
// If error, return false and pixel is set to 0.
// CAUTION : coordinates must be discrete with 12 bits precision (1 << 12).
FPXStatus PRIImage::ReadMean (long , long , Pixel& , long )
{
  return FPX_UNIMPLEMENTED_FUNCTION;
}


//  ----------------------------------------------------------------------------
// Determine the dispersion of the alpha channel in a neighborhood: the idea is to compare the local value
// to the value of the same pixel on the next resolution level. Because of the convolution from a level to
// another, one can say that if the pixels are equal, there is no dispersion of the alpha channel, otherwise
// there is some. That way, we avoid effective dispersion computation.
// CAUTION : coordinates must be discrete with 12 bits precision (1 << 12).
Boolean PRIImage::DispersionAlphaChannel (long , long , long , long , long , long , long , long , long )
{
  return (Boolean) NOT_IMPLEMENTED;
}


//  ----------------------------------------------------------------------------
// Read a rectangle (x0,y0,x1,y1: coordinates at full resolution) in a map
// The correct sub image is read and sampled in order to fill the map with the rectangle 
// CAUTION : map must be allocated and freed by calling routine
FPXStatus PRIImage::ReadSampledRectangle(long , long ,long , long , Pixel* , short , long , long , Boolean , float )
{
  return FPX_UNIMPLEMENTED_FUNCTION;
}


//  ----------------------------------------------------------------------------
// Search the top and left corner of the screen pixel which contains the given position
// This function is used when 1 image pixel is represented by more than 1 screen pixel
FPXStatus PRIImage::SearchPixelTopLeftCorner(long* , long* , float )
{
  return FPX_UNIMPLEMENTED_FUNCTION;
}


//  ----------------------------------------------------------------------------
// Compute histogram for the 4 channels 
FPXStatus PRIImage::GetHistogram (long* , long* , long* , long* , long* , const CorrectLut* )
{
  return FPX_UNIMPLEMENTED_FUNCTION;
}


//  ----------------------------------------------------------------------------
FPXStatus PRIImage::GetTrueDimensions (long* width, long* height)
{
  *height = this->height;
  *width  = this->width;
  return(FPX_OK);
}


//  ----------------------------------------------------------------------------
// Set the HotSpot coordinates
FPXStatus PRIImage::SetHotSpot (long x0, long y0)
{
  hotSpotX = x0 + cropX0; hotSpotY = y0 + cropY0;
  return FPX_OK;
}


//  ----------------------------------------------------------------------------
// Get the HotSpot coordinates
FPXStatus PRIImage::GetHotSpot (long* x0, long* y0)
{
  *x0 = hotSpotX - cropX0; *y0 = hotSpotY - cropY0;
  return FPX_OK;
}


//  - EOF ----------------------------------------------------------------------
