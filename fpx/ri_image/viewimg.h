//  ------------------------------------------------------------------------------------------------
//
//                    IMAGE VIEW TOOLKIT
//
//  MODULE    : AdvancedIVUE
//  LANGUAGE  : C++
//  AUTHOR    : Philippe BOSSUT
//  DATE    : Tuesday 23 November 1993
//  DESCRIPTION : 
//    SCCSID      : @(#)viewimg.h 1.2 14:49:52 06 Jan 1997
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
  #ifndef AdvancedIVUE_h
  #define AdvancedIVUE_h
  
#ifndef Commun_h
  #include  "common.h"
#endif

//  ------------------------------------------------------------------------------------------------

//  Includes
//  --------

#ifndef SystemIVUE_h
  #include "ri_sys.h"
#endif

#ifndef Geometrie_h
  #include "geometry.h"
#endif

#ifndef FPXBaselineIO_h
  #include "fpxlibio.h"
#endif


  // Avoid to include ColorTwist.h
    class PColorTwist;
  
//  Constants
//  ---------

  #define ViewStateNumber   5

//  Class declarations 
//  ------------------

  class PRIImage;

  class ViewWorld;
  class ViewImage;
  class ViewWindow;

  // Substructures declaration
  
  struct ViewWorldRect;
  struct ViewState;
  struct ViewTransfoState;

//  Class definitions 
//  -----------------


  // Substructures :
  // BE CAREFUL : if you derivate one of the classes where delete is redefined : the destructor should be virtual

  // World rectangles used to manage modification area optimization chained in ViewState

  struct ViewWorldRect : public PToolkitObject {
  public:
                ViewWorldRect ();
                ~ViewWorldRect ();
        
        RectangleMv   rectangle;      // Rectangle in World coordinates
        ViewWorldRect*  previous;     // Pointer to previous state rectangle
        ViewWorldRect*  next;       // Pointer to next state rectangle
  };

  struct ViewState : public PToolkitObject {
  public:   
                  ViewState (float x0, float y0, float x1, float y1);
                  ~ViewState ();
        
        void        Increment ();
        void        Increment (const PositionMv& p0, const PositionMv& p1);
        void        Increment (const RectangleMv& r);
        long        GetState ();
        RectangleMv     GetRectangle (long fromState);
  private:
        ViewWorldRect   modRectangles[ViewStateNumber]; // Array of state rectangles : modRectangles is organised as a closed loop of states
        ViewWorldRect*    lastModRectangle;       // Pointer to last modified rectangle in modRectangles
        RectangleMv     largestRectangle;       // Largest rectangle manage by state
        long        stateNumber;          // Current number of the World's state 
  };

  // Integral transformation matrices chained in Windows 

  struct ViewTransfoState : public PToolkitObject {
  public:
                  ViewTransfoState ();
                  ~ViewTransfoState ();
        
      void          Update (float x0, float y0, float resolution, ViewImage* image);
      
      long          imageState;   // State of the image when matrice was computed
      IntegralTransfo     pixelPosition;  // Direct Window pixel to image pixel transformation
      ViewTransfoState*   next;     // Pointer to the next transformation matrice
  };

  // Main objects :

  // World definition object : contains ViewImages positioned in it
  
  class ViewWorld : public PToolkitObject {
  
  friend class ViewWindow;

  public :
                  ViewWorld (float width, float height, Pixel color);
                  ~ViewWorld ();
        
        // View Images list management :
        FPXStatus     AddImage (ref_FicNom refNom);
        FPXStatus     AddImage (PRIImage* theImage);
        FPXStatus     AddImage (ViewImage* image);
          FPXStatus       DeleteImage (ViewImage* theImage, Boolean doNotCloseFile = FALSE);

        long        First (ViewImage** theImage);
        long        Last (ViewImage** theImage);
        long        Next (ViewImage** theImage);
        long        Previous (ViewImage** theImage);
        long        Current (ViewImage** theImage);
        
        long        GetState ();
  
  private :
        float       height;             // Size y of the world in mm
        float       width;              // Size x of the world in mm
        
        ViewImage*      first;              // First ViewImage within World
        ViewImage*      last;             // Last ViewImage within World
        ViewImage*      current;            // Current ViewImage within World
        
        ViewState     state;              // World state record
  };
  
  // View Image positioning definition object : point to an PRIImage object (see PRIImage.h)

  class ViewImage : public PToolkitObject {

  friend class ViewWorld;
  friend struct ViewTransfoState;
  
  public:
                 ViewImage (ref_FicNom refNom);
                 ViewImage (PRIImage* theImage);
                 ViewImage ();
        virtual     ~ViewImage ();
        
        // Read a rectangle of the transformed image
        FPXStatus   Read4x4Points(float x0, float y0,     // Rectangle in mm of the image
                        float x1, float y1, 
                        Pixel* bufPix);       // Out buffer

        // Allow access to Image members :
        PRIImage*   operator->();
        PRIImage*   GetImage() { return image; }
        FPXStatus   FileStatus ();

        // Position setting tools :       
        FPXStatus   UseAlphaChannel (Boolean useAlpha);
        FPXStatus   InvertAlphaChannel (Boolean inverseAlpha);
        FPXStatus   SetImageSize (float width, float height);
        void      SetPosition (TransfoPerspective& position);
        void      SetPosition (float x0, float y0, float m11, float m12, float m21, float m22);
        void      GetPosition (float* x0, float* y0, float* m11, float* m12, float* m21, float* m22);
        void      ApplyTransform (TransfoPerspective& trans);
        void      ApplyTransform (float x0, float y0, float m11, float m12, float m21, float m22);
        void      Translate (float dx, float dy);
        void      Rotate (float x0, float y0, float theta);
        void      Scale (float x0, float y0, float sx, float sy);
          void      ShearHorizontal (float x0, float y0, float sh);
          void      ShearVertical (float x0, float y0, float sh);
        void      FlipHorizontal (float x0, float y0);
        void      FlipVertical (float x0, float y0);
        FPXStatus   SetImageCrop(float x0, float y0, float x1, float y1);
        void      ResetImageCrop();

        // Get geometric information tools :
          long      GetOutlineParallelogram (float* x0, float* y0, float* x1, float* y1,
                               float* x2, float* y2, float* x3, float* y3);
        void      GetOutlineRectangle (float* x0, float* y0, float* x1, float* y1);
        void      GetOrigin (float* x0, float* y0);
        float       GetNormalizationRatio();
        
        long      GetState ();

        // Other parameters setting tools
        void      SetContrast (float theContrastValue);
        void      GetContrast (float* theContrastValue);
        Boolean     HasContrastValue ()  { return hasContrastValue;  }
        
        void      SetFiltering (float theFilteringValue);
        void      GetFiltering (float* theFilteringValue);
        Boolean     HasFilteringValue () { return hasFilteringValue; }
        
        long      SetFinalDimensions (float theFinalWidth, float theFinalHeight);
        void      GetFinalDimensions (float* theFinalWidth, float* theFinalHeight);
        
        void      SetColorTwist (PColorTwist* theColorTwist);
        void      GetColorTwist (PColorTwist* theColorTwist);
        Boolean     HasColorTwist ()     { return hasColorTwist;     }

        long      SetAutoResize (Boolean theAutoResize)
                      { resizeFinalDimensions = theAutoResize; return 0; }
        Boolean     GetAutoResize ()  { return resizeFinalDimensions; }
      
        long      GetDirtyCount ();

  protected:
        // Private tools : used by ViewWindow
        void      InitViewParameters();
          void      GetOutlineParallelogram (PositionMv* p0, PositionMv* p1, PositionMv* p2, PositionMv* p3);
        void      GetOutlineRectangle (PositionMv* p0, PositionMv* p1);
        void      GetOrigin (PositionMv* p0);
        
        PRIImage*   image;            // Used Image
        Boolean     doNotDelete;        // The image is not deleted if the pointer is passed
        
        // Geometric informations: these informations are mandatory in a ViewImage object
        float       originHeight, originWidth;  // Original image size within the World in mm
        float       height, width;        // Cropped image size within the World in mm
        float     cropX0, cropY0;       // Cropping rectangle 1st point in mm
        float     cropX1, cropY1;       // Cropping rectangle 2nd point in mm
      TransfoPerspective  position;         // Image position within World in mm
        Boolean     resizeFinalDimensions;    // By default, finalWidth and finalHeight are resized according to position
        float       finalHeight, finalWidth;  // Resulting image size within the World in mm
        float     normalizationRatio;     // The image is sometimes renormalized to 1 in height, this ratio allows to go back to the original height

        // Color trasnformations: these informations are optionnal
        Boolean     hasContrastValue;     // True if the contrast adjustement has to be taken into account
        float     contrastValue;        // Contrast adjustement value
        
        Boolean     hasFilteringValue;      // True if the filtering value has to be taken into account
        float     filteringValue;       // Filtering value
        
        Boolean     hasColorTwist;        // True if the color twist has to be taken into account
        PColorTwist*  colorTwist;         // Color transformation matrix
        
        ViewImage*    previous;         // Previous image within World
        ViewImage*    next;           // Next image within World
        
        ViewState   state;            // ViewImage state record

        long      dirtyCount;         // # of contrast, filtering, colorTwist, and Aspect Ratio transforms applied
  };
  
  // View Window Object : window is defined in a particular World

  class ViewWindow : public PToolkitObject {

  public :
                ViewWindow (ViewWorld* world, float x0, float y0, float width, float height, float resolution);
                ~ViewWindow ();
        
        // Basic access tools :
        FPXStatus   ReadSample (long x0, long y0, Pixel* sample, FPXBaselineColorSpace space = SPACE_32_BITS_RGB);
        FPXStatus   Refresh (Pixel* map, FPXBaselineColorSpace space, long pixWidth, long pixHeight);
        
        // Modifying Window tools :
        void      Translate (float dx, float dy);
        void      Resize (float width, float height);
        FPXStatus   Zoom (float zoomRatio);
        
        // Get usefull information :
        void      GetWindowDefinition (float* x0, float* y0, float* width, float* height, float* resolution);
        void      GetWindowDefinition (long* width, long* height);
          void      WorldToWindow (float x, float y, long* i, long* j);
          void      WindowToWorld (long i, long j, float* x, float* y);

  private :
        void      UpdateTransfoChain();   // Update integral transformation chain
        
        ViewWorld*    world;            // Pointer to viewed world
        
        long      pix_height, pix_width;    // Size of the window in real pixels
        
        float       height, width;        // Window size within the World in mm
        float     x0, y0;           // Window origin within World in mm
        float     resolution;         // Window resolution in pixel per mm
        
        Boolean     modifiedWindow;       // Flag of modification since last ReadSample()
        long      worldState;         // World state taken into account in transfo chain
    ViewTransfoState*   first;            // First pointer to integral transformation matrices
  };


// 'inline' Functions
//  -----------------

  inline ViewState::~ViewState ()
  {
  }

  inline long ViewState::GetState()
  {
    return(stateNumber);
  }
  
  inline long ViewImage::GetState()
  {
    return(state.GetState());
  }
  
  inline long ViewWorld::GetState()
  {
    return(state.GetState());
  }

  
  // ViewImage inline functions :

  inline PRIImage* ViewImage::operator->()
  {
    return(image);
  }
  
  inline float ViewImage::GetNormalizationRatio()
  {
    return normalizationRatio;
  }
  
  // Substructures inline functions
  
  inline ViewWorldRect::ViewWorldRect() : previous(0), next(0)
  {

  }
  
  inline ViewTransfoState::ViewTransfoState() : next(0)
  {

  }

  inline ViewTransfoState::~ViewTransfoState ()
  {

  }
  
  inline void ViewWindow::GetWindowDefinition (long* width, long* height)
  {
    *width  = pix_width;
    *height = pix_height;
  }


// 'extern' Variables
//  -----------------

//  ------------------------------------------------------------------------------------------------
  #endif // AdvancedIVUE_h
//  ------------------------------------------------------------------------------------------------
