// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 2001
//
// CoderInfo implementation
//

#define MAGICK_IMPLEMENTATION

#include "Magick++/CoderInfo.h"

Magick::CoderInfo::CoderInfo ( const std::string &name_ )
  : _magickInfo( 0 )
{
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  _magickInfo = GetMagickInfo( name_.c_str(), &exceptionInfo );

  // FIXME: handle error case
}

Magick::CoderInfo::~CoderInfo ( void )
{
  // Nothing to do
}

// Format name
std::string Magick::CoderInfo::name( void ) const
{
  return string(_magickInfo->name);
}

// Format description
std::string Magick::CoderInfo::description( void ) const
{
  return string(_magickInfo->description);
}

// Format is readable
bool Magick::CoderInfo::isReadable( void ) const
{
  return static_cast<bool>(_magickInfo->decoder);
}

// Format is writeable
bool Magick::CoderInfo::isWritable( void ) const
{
  return static_cast<bool>(_magickInfo->encoder);
}

// Format supports multiple frames
bool Magick::CoderInfo::isMultiFrame( void ) const
{
  return static_cast<bool>(_magickInfo->adjoin);
}
