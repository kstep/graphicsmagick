//  ----------------------------------------------------------------------------
//  MODULE    : CombinMat 
//  LANGUAGE  : C++ 
//  CREATOR   : Laurent SABORET (from Couleur.h, Adolfo VIDE, 09/15/92)
//  CREAT. DATE : Tuesday, April 30, 1996 
//  DESCRIPTION : Matrix that mixes the 3 channels of an RGB pixel 
//  COMMENTS  : 
//  SCCSID      : @(#)matrix.h  1.1 11:46:40 18 Dec 1996
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
  #ifndef CombinMat_h
  #define CombinMat_h
  #ifndef Commun_h
    #include  "common.h"
  #endif
//  ----------------------------------------------------------------------------

//  Includes
//  --------

  #include "pixel.h"
  #include "npixel.h"

//  Constants
//  ---------

//  Classes Declarations
//  --------------------

  struct CombinMat;
  typedef CombinMat* ptr_CombinMat;
  typedef CombinMat& ref_CombinMat;

//  Classes Definitions
//  -------------------

  // Matrix that mixes the 3 channels of an RGB pixel
  struct CombinMat {

              CombinMat();
        CombinMat&  operator =(const CombinMat& combinMat);

              // Apply color combinaison to a pixel
        Pixel   operator()(const Pixel& pixIn) const;
        NPixel    operator()(const NPixel& pixIn) const;
          
              // Compute a*b
    friend  CombinMat operator*(const CombinMat& a, const CombinMat& b);

        Boolean   active;
        long    coef[3][4];
  };

//  'inline' Functions
//  ------------------

//  'extern' Functions
//  ------------------

//  ----------------------------------------------------------------------------
  #endif // CombinMat_h
//  ----------------------------------------------------------------------------
