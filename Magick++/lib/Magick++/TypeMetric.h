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

    // character width, expressed in integer pixels
    unsigned int  characterWidth ( void );

    // character height, expressed in integer pixels
    unsigned int  characterHeight ( void );

    // ascent, expressed in 26.6 fixed point pixels
    int           ascent ( void );

    // descent, expressed in 26.6 fixed point pixels
    int           decent ( void );

    // text width, expressed in 26.6 fixed point pixels
    unsigned int  textWidth ( void );

    // text height, expressed in 26.6 fixed point pixels
    unsigned int  textHeight ( void );

    // maximum horizontal advance, expressed in 26.6 fixed point pixels
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
