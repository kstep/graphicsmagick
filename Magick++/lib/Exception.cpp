// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999, 2000
//
// Implementation of Exception and derived classes
//

#define MAGICK_IMPLEMENTATION

#include <string>
#include <iostream>
#include <errno.h>
#include <string.h>

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

std::ostream& operator<<( std::ostream& stream_,
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
void Magick::throwExceptionExplicit( const ExceptionType severity_,
				     const char* reason_,
				     const char* description_)
{
  // Just return if there is no reported error
  if ( severity_ == UndefinedException )
    return;

  ExceptionInfo exception;

  GetExceptionInfo( &exception );
  ThrowException( &exception, severity_, reason_, description_ );
  throwException( exception );
}

// Format and throw exception
void Magick::throwException( ExceptionInfo &exception_ )
{
  // Just return if there is no reported error
  if ( exception_.severity == UndefinedException )
    return;

  // Format error message ImageMagick-style
  std::string message = SetClientName(0);
  if ( exception_.reason != 0 )
    {
      message += std::string(": ");
      message += std::string(exception_.reason);
    }

  if ( exception_.description != 0 )
    message += " (" + std::string(exception_.description) + ")";

//   if ( error_ != OptionError && errno)
//     {
//       message += " [" + std::string(strerror(errno)) + "]";
//       errno = 0;
//     }

  
  ExceptionType severity = exception_.severity;
  DestroyExceptionInfo( &exception_ );

  switch ( severity )
    {
      // Warnings
    case ResourceLimitWarning :
      throw WarningResourceLimit( message );
    case XServerWarning :
      throw WarningXServer( message );
    case OptionWarning :
      throw WarningOption( message );
    case DelegateWarning :
      throw WarningDelegate( message );
    case MissingDelegateWarning :
      throw WarningMissingDelegate( message );
    case CorruptImageWarning :
      throw WarningCorruptImage( message );
    case FileOpenWarning :
      throw WarningFileOpen( message );
    case BlobWarning :
      throw WarningBlob ( message );
    case CacheWarning :
      throw WarningCache ( message );
      // Errors
      // FIXME: FatalException missing
    case ResourceLimitError :
      throw ErrorResourceLimit( message );
    case XServerError :
      throw ErrorXServer( message );
    case OptionError :
      throw ErrorOption( message );
    case DelegateError :
      throw ErrorDelegate( message );
    case MissingDelegateError :
      throw ErrorMissingDelegate( message );
    case CorruptImageError :
      throw ErrorCorruptImage( message );
    case FileOpenError :
      throw ErrorFileOpen( message );
    case BlobError :
      throw ErrorBlob ( message );
    case CacheError :
      throw ErrorCache ( message );
    case UndefinedException :
    default :
      throw ErrorUndefined( message );
    }

}
