// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999, 2000
//
// TypeMetric implementation
//

#define MAGICK_IMPLEMENTATION

#include "Magick++/TypeMetric.h"
#include <string.h>

// Default constructor
Magick::TypeMetric::TypeMetric ( void )
{
  memset( &_typeMetric, 0, sizeof(_typeMetric));
}

Magick::TypeMetric::~TypeMetric ( void )
{
  // Nothing to do
}

// Character width, expressed in integer pixels
unsigned int Magick::TypeMetric::characterWidth ( void )
{
  return _typeMetric.width;
}

// Character height, expressed in integer pixels
unsigned int Magick::TypeMetric::characterHeight ( void )
{
  return _typeMetric.height;
}

// Ascent, expressed in 26.6 fixed point pixels
int Magick::TypeMetric::ascent ( void )
{
  return _typeMetric.ascent;
}

// Descent, expressed in 26.6 fixed point pixels
int Magick::TypeMetric::decent ( void )
{
  return _typeMetric.descent;
}

// Text width, expressed in 26.6 fixed point pixels
unsigned int Magick::TypeMetric::textWidth ( void )
{
  return _typeMetric.width;
}

// Text height, expressed in 26.6 fixed point pixels
unsigned int Magick::TypeMetric::textHeight ( void )
{
  return _typeMetric.height;
}

// Maximum horizontal advance, expressed in 26.6 fixed point pixels
unsigned int Magick::TypeMetric::maxHorizontalAdvance ( void )
{
  return _typeMetric.max_advance;
}
