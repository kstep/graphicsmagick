//  ------------------------------------------------------------------------------------------------
//  MODULE    : Pixel
//  LANGUAGE  : C++
//  CREATION  :   04/02/96 by Laurent SABORET, from ApplCommun.h by Adolfo VIDE
//  DESCRIPTION : 
//  COMMENTS  : Mustn't include ApplCommun.h
//  SCCSID      : @(#)pixel.h 1.1 11:50:06 18 Dec 1996
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
//  ------------------------------------------------------------------------------------------------
  #ifndef Pixel_h
  #define Pixel_h
//  ------------------------------------------------------------------------------------------------

//  Includes
//  --------

#ifndef FITypes_h
  #include  "b_types.h"
#endif

//  Classes declaration
//  -------------------

  struct Pixel;
  typedef Pixel* ptr_Pixel;
  typedef Pixel& ref_Pixel;
  
//  Classes definition
//  ------------------

  // Type of a 32 bits pixel. 
  // This type matches a pixel in the Macintosh video RAM, so don't modify it!
  struct Pixel {
  
              Pixel();
              Pixel(int32 rgb);

        operator    int32 ();

        unsigned char alpha;            // Alpha channel
        unsigned char rouge;            // Red
        unsigned char vert;           // Green
        unsigned char bleu;           // Blue
  };
  
//  Inline functions
//  ----------------

  inline Pixel::Pixel() { }
  
  inline Pixel::Pixel(int32 rgb)
  
  {
    *((int32 *)this) = rgb;
  }
  
  inline Pixel::operator int32() {
    return *((int32*)(this));
  }

//  ------------------------------------------------------------------------------------------------
  #endif // Pixel_h
//  ------------------------------------------------------------------------------------------------
