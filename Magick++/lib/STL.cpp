// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999, 2002
//
// Implementation of STL classes and functions
//

#include <Magick++/Image.h>
#include <Magick++/STL.h>
#define MAGICK_IMPLEMENTATION

Magick::addNoiseImage::addNoiseImage( Magick::NoiseType noiseType_ )
  : _noiseType( noiseType_ )
{
}

void Magick::addNoiseImage::operator()( Magick::Image &image_ )
{
  image_.addNoise( _noiseType );
}
