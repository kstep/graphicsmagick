//  ----------------------------------------------------------------------------
//  MODULE    : ColorTwist 
//  LANGUAGE  : C++
//  CREATOR   : Philippe BOSSUT 
//  CREAT. DATE : Monday, March 25, 1996 
//  DESCRIPTION :  
//  COMMENTS  : 
//    SCCSID        : @(#)coltwist.h  1.2 12:44:00 29 Apr 1997
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
  #ifndef ColorTwist_h
  #define ColorTwist_h
  #ifndef Commun_h
    #include  "common.h"
  #endif
//  ----------------------------------------------------------------------------

//  Includes
//  --------

#ifndef SystemIVUE_h
  #include  "ri_sys.h"
#endif

#ifndef Pixel_h
  #include  "pixel.h"
#endif

// Avoid to include FPXBaselineIO.h
struct FPXColorTwistMatrix;

//  Constants
//  ---------

//  Types Declarations
//  ------------------

// Define different types of transformation used in FlashPix. See Appendix B: Colortwist matrices
// for a definition of these transformations and an explanation of the notation used here.

typedef enum { ycc_to_YCC8,   // Normalized PhotoYCC to 8-bit PhotoYCC
         YCC8_to_ycc,   // 8-bit PhotoYCC to normalized PhotoYCC
         ycc_to_rgb,    // Normalized PhotoYCC to normalized PhotoRGB
         rgb_to_ycc,    // Normalized PhotoRGB to normalized PhotoYCC
         RGB8_to_rgb,   // 8-bit NIF RGB to normalized PhotoRGB
         rgb_to_RGB8,   // Normalized PhotoRGB to 8-bit NIF RGB
         rgb_to_mono    // 8-bit NIF RGB to monochrome - PTCH_302
} FPXTransfoColor;

//  Types Definitions
//  -----------------

//  Classes Declarations
//  --------------------

  class PColorTwist;

//  Classes Definitions
//  -------------------

  // PColorTwist is defined as a canonic class which means it contains:
  //    - a default constructor
  //    - a copy constructor
  //    - a destructor
  //    - a copy operator

  class PColorTwist : public PToolkitObject {
  
    public:
                 PColorTwist ();
                 PColorTwist (FPXTransfoColor transfo);
                 PColorTwist (const PColorTwist& T);
                 PColorTwist (const FPXColorTwistMatrix& T);
                 PColorTwist (float t11, float t12, float t13, float t14,
                        float t21, float t22, float t23, float t24,
                        float t31, float t32, float t33, float t34,
                        float t44);
                ~PColorTwist ();

        // Composition operators
        PColorTwist&  operator= (const PColorTwist& T);
  friend    PColorTwist   operator* (const PColorTwist& T1, const PColorTwist& T2);
        PColorTwist&  operator*=(const PColorTwist& T);
//  friend    PColorVector  operator * (const PColorTwist& T, const PColorVector& color);

        // Inverse the color transformation
        void      Inverse();

        void      ApplyToBuffer(unsigned char* buffer, long size);
        void      ApplyRGBtoYCCLut(unsigned char* buffer, long size);
        Boolean     ApplyToPixelBuffer(Pixel* pixels, FPXBaselineColorSpace colorSpace, long size);
        void      UsePortfolioLut(void);
        void      ConvertToFPX (FPXColorTwistMatrix* T);
        Boolean     IsIdentity();
        void      UseAlphaChannel();
    private:
        float     T11 , T12, T13, T14;
        float     T21 , T22, T23, T24;
        float     T31 , T32, T33, T34;
        float     T44;

        Boolean     applyPortfolioLut;
        Boolean     applyAlpha;
  };

//  'inline' Functions
//  ------------------

//  'extern' Functions
//  ------------------

//  'extern' Variables
//  ------------------

//  ----------------------------------------------------------------------------
  #endif // ColorTwist_h
//  ----------------------------------------------------------------------------
