//  ------------------------------------------------------------------------------------------------
//
//                     IMAGE OUTPUT PAGE MANAGEMENT
//
//  This source code is the property of FITS Imaging inc.
//
//  MODULE    : PageIVUE
//  LANGUAGE  : C++
//  AUTHOR    : Philippe BOSSUT
//  DATE    : Thursday, December 16th 1993
//  DESCRIPTION : 
//  COMMENT   : This package is a part of the IVUE Toolkit I/O and the FlashPix Baseline I/O Toolkit
//  SCCSID      : @(#)ri_page.h 1.2 15:08:08 14 Apr 1997
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
//  ------------------------------------------------------------------------------------------------
  #ifndef PageIVUE_h
  #define PageIVUE_h
#ifndef Commun_h
  #include  "common.h"
#endif

//  ------------------------------------------------------------------------------------------------

//  Includes
//  --------
#ifndef Geometrie_h
  #include "geometry.h"
#endif

#ifndef SystemIVUE_h
  #include "ri_sys.h"
#endif

#ifndef AdvancedIVUE_h
  #include "viewimg.h"
#endif

#ifndef FPXBaselineIO_h
  #include "fpxlibio.h"
#endif

//  Constants
//  ---------

//  Class Declarations 
//  ------------------

  class PageImage;

  // For compatibility with already existing LivePicture code
  typedef PageImage* ptr_PageIvue;
  typedef PageImage& ref_PageIvue;

//  Class Definitions 
//  -----------------

  // Output Page Handle
  // This is used only in the Standard Toolkit to produce formatted output page
  // See AdvancedIVUE.h for more complex and versatile output possibilities

  class PageImage : public PToolkitObject {
  
  public:
                PageImage (PRIImage* rawImage, long width, long height, float rotation);
                PageImage (PRIImage* rawImage,      // Image file
                      long width, long height,    // Size of the page in pixels
                      float resolution,       // Page's resolution in pixels/mm
                      float x, float y,       // Page's origin in the World (mm)
                      TransfoPerspective transfo);  // Image position within the page
                PageImage (ViewImage* image, long width, long height, float rotation);
                PageImage (ViewImage* image,      // Image file
                      long width, long height,    // Size of the page in pixels
                      float resolution,       // Page's resolution in pixels/mm
                      float x, float y,       // Page's origin in the World (mm)
                      TransfoPerspective transfo);  // Image position within the page
        virtual     ~PageImage ();
        
        // Read the all page
        FPXStatus     ReadPage (Pixel* image);
        // Read a line of the page
        FPXStatus   ReadPageLine (long lineNumber, Pixel* line);
        // Read a rectangle inside the page
        // PTCH_105 - added 'reportProgress' argument to ReadRectangle()
        FPXStatus   ReadRectangle(long x0, long y0,     // Rectangle in pixels inside the page
                        long x1, long y1, 
                        Pixel* bufPix,      // Out buffer
                        long bufWidth,      // Size of a buffer's line
                        Boolean withAntialias = false,  // if true use the antialiasing
                        Boolean reportProgress = true); // if true call progress function
        // Get page's parameters                  
        void      GetPixelSize(long* w, long* h);
        void      GetResolution(float* r);
        PRIImage*   GetImage();
        
        // Change the page's Image file
        void      SetImage(ViewImage* theImage);
        
  private:

        void      ComputeRotationMatrix (TransfoPerspective* position, float rotation);
        
        // Read without interleaving and give a pointer on the line
        Pixel*      ReadLine (long lineNumber, FPXStatus* status);  // read a line in the page
        
        long      pixHeight, pixWidth;    // Size of the page in pixels
        
        ViewImage*    image;            // Used Image View
        PRIImage*   rawImage;         // Used Image Object
        float     xOrigin, yOrigin;     // Origin of the page
        float     resolution;         // Resolution in pixels by mm
        
        // buffers and variables used to read the lines 4 by 4 and to not read the same line twice 
        long      numLine;          // Number of the first line of the line buffer
        long      lineSize;         // Length of the 4 lines
        Pixel*      buffer;           // Buffers of the 4 lines
        Pixel*      line[4];          // Pointers of the 4 lines in buffer
  };

//  'inline' Functions 
//  ------------------

inline void PageImage::GetPixelSize(long* w, long* h)

{
  *w = pixWidth;
  *h = pixHeight;
}

inline void PageImage::GetResolution(float* r)

{
  *r = resolution;
}

inline PRIImage* PageImage::GetImage()

{
  return image->GetImage();
}

inline void PageImage::SetImage(ViewImage* theImage)

{
  image = theImage;
}

// 'extern' Functions
//  -----------------

// 'extern' Variables
//  -----------------

//  ------------------------------------------------------------------------------------------------
  #endif // PageIVUE_h
//  ------------------------------------------------------------------------------------------------
