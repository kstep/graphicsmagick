// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999, 2000, 2001
//
// Simple C++ function wrappers for often used or otherwise
// inconvenient ImageMagick equivalents
//

#if !defined(Magick_Functions_header)
#define Magick_Functions_header

#include <string>

#include "Magick++/Include.h"

namespace Magick
{
  void CloneString( char **destination_, const std::string &source_ );

}
#endif // Magick_Functions_header
