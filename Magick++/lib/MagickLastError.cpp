// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999
//
// Implementation of MagickError (ImageMagick error handler)
//

#define MAGICK_IMPLEMENTATION

#include <string>

#include <errno.h>

#include <MagickInclude.h>

using namespace std;

#include <MagickLastError.h>
#include <MagickException.h>

// Magick++ combined error/warning callback function
namespace Magick
{
  extern "C" {
  void LastErrorHandler( const unsigned int error_,
			 const char *message_ ,
			 const char *qualifier_);
  }
}

// Initialize static members
Magick::LastError*  Magick::LastError::_instance = 0;

// Constructor
Magick::LastError::LastError( void )
  : _isError(false),
    _error(0),
    _syserror(0),
    _message(),
    _qualifier()
{
}

// Construct with parameters
Magick::LastError::LastError( int error_, const std::string message_,
			      const std::string qualifier_ )
  : _isError(true),
    _error(error_),
    _syserror(errno),
    _message(message_),
    _qualifier(qualifier_)
{
}

// Destructor
Magick::LastError::~LastError( void )
{
  // Nothing to do
}

// Obtain instance of object
Magick::LastError* Magick::LastError::instance ( void )
{
  if ( _instance == 0 )
    {
      // Assuming that new throws a bad_alloc exception here if
      // allocation error occurs
      _instance = new LastError();

      // Register error callback function with ImageMagick
      MagickLib::SetErrorHandler( LastErrorHandler );
      MagickLib::SetWarningHandler( LastErrorHandler );
    }
  return _instance;
}

// Test to see if object contains error
void Magick::LastError::isError( bool isError_ )
{
  _isError = isError_;
}
bool Magick::LastError::isError( void ) const
{
  return _isError;
}

// Clear out existing error info
void Magick::LastError::clear ( void )
{
  _isError   = false;
  _error     = 0;
  _syserror  = 0;
  _message.erase();
  _qualifier.erase();
}

// Error code
void Magick::LastError::error ( int error_ )
{
  _error = error_;
}
int Magick::LastError::error ( void ) const
{
  return _error;
}

void Magick::LastError::syserror ( int syserror_ )
{
  _syserror = syserror_;
}
int Magick::LastError::syserror ( void ) const
{
  return _syserror;
}
  

// Error message
void Magick::LastError::message ( std::string message_ )
{
  _message = message_;
}
std::string Magick::LastError::message ( void ) const
{
  return _message;
}

// Error qualifier
void  Magick::LastError::qualifier ( std::string qualifier_ )
{
  _qualifier = qualifier_;
}
std::string Magick::LastError::qualifier ( void ) const
{
  return _qualifier;
}

// Throw exception corresponding to error (if any)
void Magick::LastError::throwException( void ) const
{
  if ( !isError() )
    return;

  // Format error message ImageMagick-style
  std::string message = MagickLib::SetClientName((char *) NULL);
  if ( message.length() > 0 )
    message += std::string(": ");

  if ( _message.length() > 0 )
    message +=  _message;

  if ( _qualifier.length() > 0 )
    message += " (" + _qualifier + ")";

  // FIXME: For some reason this all of a sudden became necessary on a SVR4 box!
#undef strerror
  if ( syserror() )
    message += std::string(" [") + strerror(syserror()) + std::string("]");

  switch ( _error )
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
      // Errors
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
    case MagickLib::UndefinedError :
    default :
      throw ErrorUndefined( message );
    }
}

//
// Magick++ combined error/warning callback function
//
void Magick::LastErrorHandler( const unsigned int error_,
			       const char *message_ ,
			       const char *qualifier_)
{
  LastError* errPtr = LastError::instance();
  errPtr->isError( true );
  errPtr->error( error_ );
  errPtr->syserror( errno );
  if ( message_ )
    errPtr->message( message_ );
  else
    errPtr->message( "" );
  if ( qualifier_ )
    errPtr->qualifier( qualifier_ );
  else
    errPtr->qualifier( "" );

  // Clear out system errno now that it has been collected.
  errno = 0;
}
