// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999, 2002
//
// Simple C++ function wrappers for ImageMagick equivalents
//

#define MAGICK_IMPLEMENTATION

#include <string>
#include <iostream>

using namespace std;

#include "Magick++/Functions.h"

// Clone C++ string as allocated C string, de-allocating any existing string
void Magick::CloneString( char **destination_, const std::string &source_ )
{
  CloneString( destination_, source_.c_str() );
}

