// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 2001
//
// CoderInfo Definition
//
// Container for image format coder information
//

#if !defined (Magick_CoderInfo_header)
#define Magick_CoderInfo_header

#include "Magick++/Include.h"
#include <string>

namespace Magick
{
  class CoderInfo
  {
  public:
    
    CoderInfo ( const std::string &name_ );
    ~CoderInfo ( void );

    // Format name
    std::string name( void ) const;

    // Format description
    std::string description( void ) const;

    // Format is readable
    bool isReadable( void ) const;

    // Format is writeable
    bool isWritable( void ) const;

    // Format supports multiple frames
    bool isMultiFrame( void ) const;

  private:

    // Default constructor
    CoderInfo::CoderInfo ( void );

    // Copy constructor
    CoderInfo::CoderInfo ( const CoderInfo &coder_ );

    // Assignment operator
    CoderInfo& operator= (const CoderInfo &coder_ );

    MagickLib::MagickInfo	*_magickInfo;
  };
} // namespace Magick

//
// Inlines
//


#endif // Magick_CoderInfo_header
