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

    // Ascent, the distance in pixels from the text baseline to the
    // highest/upper grid coordinate used to place an outline point.
    double         ascent ( void );

    // Descent, the distance in pixels from the baseline to the lowest
    // grid coordinate used to place an outline point. Always a
    // negative value.
    double         decent ( void );

    // Text width in pixels.
    double         textWidth ( void );

    // Text height in pixels.
    double         textHeight ( void );

    // Maximum horizontal advance in pixels.
    double         maxHorizontalAdvance ( void );

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
