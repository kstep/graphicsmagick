// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 2001
//
// TypeMetric Definition
//
// Container for font type metrics
//

#if !defined (Magick_TypeMetric_header)
#define Magick_TypeMetric_header

#include "Magick++/Include.h"

namespace Magick
{
  class TypeMetric
  {
    friend class Image;
  public:
    
    TypeMetric ( void );
    ~TypeMetric ( void );

    // Character width, expressed in integer pixels
    unsigned int  characterWidth ( void );

    // Character height, expressed in integer pixels
    unsigned int  characterHeight ( void );

    // Ascent, the distance from the baseline to the highest/upper
    // grid coordinate used to place an outline point. Always a
    // positive value.  Expressed in 26.6 fixed point pixels
    int           ascent ( void );

    // Descent, the distance from the baseline to the lowest grid
    // coordinate used to place an outline point. Always a negative
    // value.  Expressed in 26.6 format (i.e. in 1/64ths of a pixel)
    int           decent ( void );

    // Text width. Expressed in 26.6 format (i.e. in 1/64ths of a
    // pixel).
    unsigned int  textWidth ( void );

    // Text height. Expressed in 26.6 format (i.e. in 1/64ths of a
    // pixel).
    unsigned int  textHeight ( void );

    // Maximum horizontal advance. Expressed in 26.6 format (i.e. in
    // 1/64ths of a pixel).
    unsigned int  maxHorizontalAdvance ( void );

    //
    // Public methods below this point are for Magick++ use only.
    //

  private:
    MagickLib::TypeMetric  _typeMetric;
  };
} // namespace Magick

//
// Inlines
//


#endif // Magick_TypeMetric_header
