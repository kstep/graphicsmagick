// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 2001
//
// CoderInfo Definition
//
// Return format coder details.
//

#if !defined (Magick_CoderInfo_header)
#define Magick_CoderInfo_header

#include "Magick++/Include.h"
#include <string>

namespace Magick
{
  enum MatchType {
    AnyMatch,	// match any coder
    TrueMatch,	// match coder if true
    FalseMatch	// match coder if false
  };

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

    //
    // Implemementation methods
    //
    CoderInfo ( const MagickLib::MagickInfo *magickInfo_ );

  private:

    // Default constructor (not supported)
    CoderInfo::CoderInfo ( void );

    // Copy constructor (not supported)
//     CoderInfo::CoderInfo ( const CoderInfo &coder_ );

    // Assignment operator (not supported)
    CoderInfo& operator= (const CoderInfo &coder_ );

    std::string		_name;
    std::string		_description;
    bool		_isReadable;
    bool		_isWritable;
    bool		_isMultiFrame;
    
  };
} // namespace Magick

//
// Inlines
//


#endif // Magick_CoderInfo_header
