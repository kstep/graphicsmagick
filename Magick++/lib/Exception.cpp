// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999
//
// Implementation of Exception and derived classes
//

#define MAGICK_IMPLEMENTATION

#include <string>

using namespace std;

#include "Magick++/Exception.h"

// Construct with message string
Magick::Exception::Exception( const std::string& what_ )
  : _what(what_)
{
}

// Return message string
const char* Magick::Exception::what( ) const throw()
{
  return _what.c_str();
}


// Print exception to stream.
std::ostream& Magick::operator<<( std::ostream& stream_,
				  const Magick::Exception& exception_)
{
  stream_ << exception_.what();
  return stream_;
}


//
// Warnings
//

Magick::Warning::Warning ( const std::string& what_ )
  : Exception(what_)
{
}

Magick::WarningUndefined::WarningUndefined ( const std::string& what_ )
  : Warning(what_)
{
}

Magick::WarningResourceLimit::WarningResourceLimit ( const std::string& what_ )
  : Warning(what_)
{
}

Magick::WarningXServer::WarningXServer ( const std::string& what_ )
  : Warning(what_)
{
}

Magick::WarningOption::WarningOption ( const std::string& what_ )
  : Warning(what_)
{
}

Magick::WarningDelegate::WarningDelegate ( const std::string& what_ )
  : Warning(what_)
{
}

Magick::WarningMissingDelegate::WarningMissingDelegate ( const std::string& what_ )
  : Warning(what_)
{
}

Magick::WarningCorruptImage::WarningCorruptImage ( const std::string& what_ )
  : Warning(what_)
{
}

Magick::WarningFileOpen::WarningFileOpen ( const std::string& what_ )
  : Warning(what_)
{
}

Magick::WarningBlob::WarningBlob ( const std::string& what_ )
  : Warning(what_)
{
}

Magick::WarningCache::WarningCache ( const std::string& what_ )
  : Warning(what_)
{
}

//
// Errors
//

Magick::Error::Error ( const std::string& what_ )
  : Exception(what_)
{
}

Magick::ErrorUndefined::ErrorUndefined ( const std::string& what_ )
  : Error(what_)
{
}

Magick::ErrorResourceLimit::ErrorResourceLimit ( const std::string& what_ )
  : Error(what_)
{
}

Magick::ErrorXServer::ErrorXServer ( const std::string& what_ )
  : Error(what_)
{
}

Magick::ErrorOption::ErrorOption ( const std::string& what_ )
  : Error(what_)
{
}

Magick::ErrorDelegate::ErrorDelegate ( const std::string& what_ )
  : Error(what_)
{
}

Magick::ErrorMissingDelegate::ErrorMissingDelegate ( const std::string& what_ )
  : Error(what_)
{
}

Magick::ErrorCorruptImage::ErrorCorruptImage ( const std::string& what_ )
  : Error(what_)
{
}

Magick::ErrorFileOpen::ErrorFileOpen ( const std::string& what_ )
  : Error(what_)
{
}

Magick::ErrorBlob::ErrorBlob ( const std::string& what_ )
  : Error(what_)
{
}

Magick::ErrorCache::ErrorCache ( const std::string& what_ )
  : Error(what_)
{
}

// Format and throw exception
void Magick::throwException( MagickLib::ExceptionType severity_,
			     const char* message_,
			     const char* qualifier_)
{
  // Just return if there is no reported error
  if ( severity_ == MagickLib::UndefinedException )
    return;

  // Format error message ImageMagick-style
  std::string message = MagickLib::SetClientName((char *) NULL);
  if ( message_ )
    {
      message += std::string(": ");
      message += std::string(message_);
    }

  if ( qualifier_ )
    message += " (" + std::string(qualifier_) + ")";

  switch ( severity_ )
    {
      // Warnings
    case MagickLib::ResourceLimitWarning :
      throw WarningResourceLimit( message );
    case MagickLib::XServerWarning :
      throw WarningXServer( message );
    case MagickLib::OptionWarning :
      throw WarningOption( message );
    case MagickLib::DelegateWarning :
      throw WarningDelegate( message );
    case MagickLib::MissingDelegateWarning :
      throw WarningMissingDelegate( message );
    case MagickLib::CorruptImageWarning :
      throw WarningCorruptImage( message );
    case MagickLib::FileOpenWarning :
      throw WarningFileOpen( message );
    case MagickLib::BlobWarning :
      throw WarningBlob ( message );
    case MagickLib::CacheWarning :
      throw WarningCache ( message );
      // Errors
      // FIXME: FatalException missing
    case MagickLib::ResourceLimitError :
      throw ErrorResourceLimit( message );
    case MagickLib::XServerError :
      throw ErrorXServer( message );
    case MagickLib::OptionError :
      throw ErrorOption( message );
    case MagickLib::DelegateError :
      throw ErrorDelegate( message );
    case MagickLib::MissingDelegateError :
      throw ErrorMissingDelegate( message );
    case MagickLib::CorruptImageError :
      throw ErrorCorruptImage( message );
    case MagickLib::FileOpenError :
      throw ErrorFileOpen( message );
    case MagickLib::BlobError :
      throw ErrorBlob ( message );
    case MagickLib::CacheError :
      throw ErrorCache ( message );
    case MagickLib::UndefinedException :
    default :
      throw ErrorUndefined( message );
    }
}

// Format and throw exception
void Magick::throwException( const MagickLib::ExceptionInfo &exception_ )
{

  // Just return if there is no reported error
  if ( exception_.severity == MagickLib::UndefinedException )
    return;

  throwException( exception_.severity,
		  exception_.message,
		  exception_.qualifier );
}
