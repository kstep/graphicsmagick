/*  FPXBaselineView.h
 *
 *  Copyright (c) 1999 Digital Imaging Group, Inc.
 *  For conditions of distribution and use, see copyright notice
 *  in Flashpix.h
 *
 *  FlashPix Baseline View Layer API
 *
 */
//  SCCSID      : @(#)fpxlib.h  1.2 08:41:22 29 Jan 1997

/****************************************************************************/
  #ifndef FPXBaselineView_h
  #define FPXBaselineView_h
/****************************************************************************/

#ifndef FPXBaselineIO_h
  #include "fpxlibio.h"              /* for various I/O level functions. */
#endif

//  Viewing Toolkit Class Definitions
//  ---------------------------------

class PageImage;
typedef PageImage FPXPage;
class ViewWorld;
typedef ViewWorld FPXWorld;
class ViewImage;
typedef ViewImage FPXImageInWorld;
class ViewWindow;
typedef ViewWindow FPXWindow;

extern "C" {

// Formatted output tools :
// Perhaps misnamed. Actually should be called simple render or
// simple print. provide quick means of rendering a single image.
// allows for simple rotation. Auto scales to maximize rotated 
// image size in page without crop.

FPXEXPORT FPXStatus FPX_SetPageSetup (
              FPXImageHandle* theFPX, 
              FPXPage**       thePage, 
              long            width, 
              long            height,
              float           rotation,
              FPXColorspace   backgroundColorspace,
              FPXBackground   backgroundColor);
             
                        
FPXEXPORT FPXStatus FPX_ClosePage (
              FPXPage*      thePage);
             
                        
FPXEXPORT FPXStatus FPX_ReadPage (
              FPXPage*      thePage, 
              FPXImageDesc* renderingBuffer);

                        
FPXEXPORT FPXStatus FPX_ReadPageLine (
              FPXPage*      thePage, 
              long          lineNumber, 
              FPXImageDesc* renderingBuffer);
                           

} // extern "C"
        
extern "C" {

// World tools :
                        
FPXEXPORT FPXStatus FPX_CreateWorld (
              FPXWorld**    theWorld, 
              float         width, 
              float         height,
              FPXColorspace backgroundColorspace,
              FPXBackground backgroundColor);
            
FPXEXPORT FPXStatus FPX_DeleteWorld (
              FPXWorld*     theWorld);
        
// the following several functions are for managing multiple images
// in a world.
// Should we wish to support multiple FPX images in a world,
// we will need these functions.

// List of images management :

FPXEXPORT FPXStatus FPX_AddImage (
              FPXWorld*         theWorld, 
              FPXImageInWorld** theImage,
              FPXImageHandle*   theFPX);
                
// Adds an image to the world. returns handle to the 
// FPXImageInWorld.
// The image is placed as specified by the affine 
// matrix in it's view.
                                
FPXEXPORT FPXStatus FPX_DeleteImage (
              FPXWorld*        theWorld, 
              FPXImageInWorld* theImage);
                
// removes an image from the world.
// does not remove associated window created at 
// FPX_AddImage time.
                
FPXEXPORT FPXStatus FPX_GetFirstImage (
              FPXWorld*         theWorld, 
              FPXImageInWorld** theImage);
                
// get first image from world image list.
                                
FPXEXPORT FPXStatus FPX_GetLastImage (
              FPXWorld*         theWorld, 
              FPXImageInWorld** theImage);
                
// get last image from world image list.
                                
FPXEXPORT FPXStatus FPX_GetCurrentImage (
              FPXWorld*         theWorld, 
              FPXImageInWorld** theImage);
                
// get current image from world image list.
                
FPXEXPORT FPXStatus FPX_NextImage (
              FPXWorld*         theWorld, 
              FPXImageInWorld** theImage);
                
// get the next image in the list. becomes current.
                                
FPXEXPORT FPXStatus FPX_PreviousImage (
              FPXWorld*         theWorld, 
              FPXImageInWorld** theImage);

// get the previous image in the list. becomes current.


// misc.

// Image position tools :
FPXEXPORT FPXStatus FPX_SetImageSize (
              FPXImageInWorld* theImage, 
              float            width, 
              float            height);

// Set width and height of image in world coord.s
// I.e. scale in x and y.

FPXEXPORT FPXStatus FPX_SetImageTransform (
              FPXImageInWorld* theImage, 
              float x0,  float y0, 
              float m11, float m12, float m21, float m22);
                    
// Set full 2D affine in one swell foop.
        
FPXEXPORT FPXStatus FPX_ApplyImageTransform (
              FPXImageInWorld* theImage, 
              float x0,  float y0, 
              float m11, float m12, float m21, float m22);
                    
// Compose the specified affine with the existing affine xfrm 
// of the image.
        
FPXEXPORT FPXStatus FPX_TranslateImage (
              FPXImageInWorld* theImage, 
              float dx, float dy);
           
// Move the image in world coord.s. will form new rendering xform.
// Composed with existing xform.
        
FPXEXPORT FPXStatus FPX_RotateImage (
              FPXImageInWorld* theImage, 
              float x0, float y0,
              float theta);
                                 
// Rotate an image ccw about a given point.
// Implemented as xlate of said point to origin, rotate, 
// xlate back.
        
FPXEXPORT FPXStatus FPX_ScaleImage (
              FPXImageInWorld* theImage, 
              float x0, float y0,
              float sx, float sy);
                                
// Scale in x and y about a specific point.
// Implemented as xlation of said point to origin, scale, 
// xlate back.

FPXEXPORT FPXStatus FPX_ShearHorizontal (
              FPXImageInWorld* theImage, 
              float x0, float y0, 
              float sh);
                                     
// Compose a horizontal shear on to the xform.
// Shear is wrt a given point. Implemented by translation of
// said point to the origin, shear, translation back.
        
FPXEXPORT FPXStatus FPX_ShearVertical (
              FPXImageInWorld* theImage, 
              float x0, float y0, 
              float sh);
           
// Compose a vertical shear on to the xform.
// Shear is wrt a given point. Implemented by translation of
// said point to the origin, shear, translation back.
        
FPXEXPORT FPXStatus FPX_FlipHorizontal (
              FPXImageInWorld* theImage, 
              float x0, float y0);
           
// Flip (mirror) image about a horzontal line.
// Said line runs through x0,y0.
// Implemented by xlate of point to origin, scale x by -1, 
// xlate back.

FPXEXPORT FPXStatus FPX_FlipVertical (
              FPXImageInWorld* theImage, 
              float x0, float y0);
           
// Flip (mirror) image about a vertical line.
// Said line runs through x0,y0.
// Implemented by xlate of point to origin, scale x by -1, 
// xlate back.
        
FPXEXPORT FPXStatus FPX_GetOutlineParallelogram (
              FPXImageInWorld* theImage, 
              float* x0, float* y0, 
              float* x1, float* y1,
              float* x2, float* y2, 
              float* x3, float* y3);
                                                                                        
// Return corners of an image as rendered.
// The bounding box including shear and rotation and crop (ROI).
        
FPXEXPORT FPXStatus FPX_GetOutlineRectangle (
              FPXImageInWorld* theImage, 
              float* x0, float* y0, 
              float* x1, float* y1);

// Get the corners of a 0 rotation rectangle which bounds the 
// image. Conventional bounding box.
        
FPXEXPORT FPXStatus FPX_GetOrigin (
              FPXImageInWorld* theImage, 
              float* x0, float* y0);
           
// return coord.s of origin of the image (upper left)
// == translation values from xfrm. I.e. location of the 
// upper left.
        
FPXEXPORT FPXStatus FPX_SetImageCrop (
              FPXImageInWorld* theImage, 
              float x0, float y0, 
              float x1, float y1);
                                  
// Set Region of Interest (ROI) on image.
// Coords. are in world coords.
        
FPXEXPORT FPXStatus FPX_ResetImageCrop (
              FPXImageInWorld* theImage);

// Set ROI to full image.

// Viewing options: this concern the usage of the alpha channel stored along
// with the image in the composition:

FPXEXPORT FPXStatus FPX_UseAlphaChannel (
              FPXImageInWorld* theImage,
              FPXbool          useAlphaChannel);
// The alpha channel is taken into account if (useAlphaChannel == true)

FPXEXPORT FPXStatus FPX_InvertAlphaChannel (
              FPXImageInWorld* theImage,
              FPXbool          inverseAlpha);
// The opacity is inversed prior composition if (inverseAlpha == true)
    
// View Window  tools :
// An FPX window is really a viewport on the world.
// It establishes resolution and hence pixels.
                        
FPXEXPORT FPXStatus FPX_CreateWindow (
              FPXWorld*    theWorld, 
              FPXWindow**  theWindow, 
              float x0,    float y0, 
              float width, float height, 
              float resolution);
                                  
FPXEXPORT FPXStatus FPX_DeleteWindow (
              FPXWindow* theWindow);

FPXEXPORT FPXStatus FPX_ReadWindowSample (
              FPXWindow*    theWindow, 
              long x0,      long y0,
              FPXImageDesc* windowBufferInfo);
                                      
// Render one lump of output.
// This is a 4x4 square about the specified point (in pixels) 
// in the window.
                        
FPXEXPORT FPXStatus FPX_RefreshWindow (
              FPXWindow*    theWindow, 
              FPXImageDesc* windowBufferInfo);
           
// Render the entire window at once.
// Includes a progress Function, as rendering may take a while, 
// especially at high res.
        
FPXEXPORT FPXStatus FPX_TranslateWindow (
              FPXWindow* theWindow, 
              float  dx, float  dy);
           
// Move a window in world coords.
// Changes xform associated with window.
        
FPXEXPORT FPXStatus FPX_ResizeWindow (
              FPXWindow*   theWindow, 
              float width, float height);
           
// Crop or enlarge a window, without changing resolution.
// I.e. no zooming, just a bigger window with more pixels in it.
        
FPXEXPORT FPXStatus FPX_ZoomWindow (
              FPXWindow* theWindow, 
              float      zoomRatio);
           
// Change window dimensions and resolution inversely so as 
// to zoom/dezoom.
        
FPXEXPORT FPXStatus FPX_GetWindowDefinition (
              FPXWindow*    theWindow, 
              float* x0,    float* y0, 
              float* width, float* height, 
              float* resolution);
                                         
// Get vital statistics about a window: location, size, 
// resolution.
        
FPXEXPORT FPXStatus FPX_WindowToWorld (
              FPXWindow* theWindow, 
              long i,    long j, 
              float* x,  float* y);
                                   
// Xform point from window (pixels) to world coords.
        
FPXEXPORT FPXStatus FPX_WorldToWindow (
              FPXWindow* theWindow, 
              float x, float y, 
              long* i, long* j);
                                   
// Xform point from world coords. to window coord.s (pixels.)


//      Contrast adjustment:
//              Setting, getting, processing.
//              Note that this is really a property of a View of an image.
//              We will associate it with the image in the world.
/*
        Contrast Adjustment
*/

FPXEXPORT FPXStatus FPX_SetImageInWorldContrastAdjustment (
              FPXImageInWorld*       theImage,
              FPXContrastAdjustment* theContrastAdjustment);
        
FPXEXPORT FPXStatus FPX_GetImageInWorldContrastAdjustment (
              FPXImageInWorld*       theImage,
              FPXContrastAdjustment* theContrastAdjustment);
        
        
//      Color twist:
//              Setting, getting, processing.
/*
        ColorTwist Matrix */

FPXEXPORT FPXStatus FPX_SetImageInWorldColorTwistMatrix (
              FPXImageInWorld*     theImage,
              FPXColorTwistMatrix* theColorTwistMatrix);
        
FPXEXPORT FPXStatus FPX_GetImageInWorldColorTwistMatrix (
              FPXImageInWorld*     theImage,
              FPXColorTwistMatrix* theColorTwistMatrix);
        
                
//      Sharpening:
//              Setting, getting, processing.
//              This is defined to occur after Image Decompression, but
//              before color transformation/twisting/constrast adjustment.

/*        Filtering. */

FPXEXPORT FPXStatus FPX_SetImageInWorldFilteringValue (
              FPXImageInWorld*   theImage,
              FPXFilteringValue* theFiltering);
        
FPXEXPORT FPXStatus FPX_GetImageInWorldFilteringValue (
              FPXImageInWorld*   theImage,
              FPXFilteringValue* theFiltering);

/* Get current Image in World Affine Matrix */

FPXEXPORT FPXStatus FPX_GetImageInWorldAffineMatrix (
      FPXImageInWorld*    theImage,
      FPXAffineMatrix*  mat);
      
/* Set current Image in World Affine Matrix */

FPXEXPORT FPXStatus FPX_SetImageInWorldAffineMatrix (
      FPXImageInWorld*    theImage,
      FPXAffineMatrix*  mat);


} // extern "C"
                
/****************************************************************************/
  #endif // FPXBaselineView_h
/****************************************************************************/

